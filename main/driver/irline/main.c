/**
 * @file main.c
 * @author Ang Jia Yu
 * @brief Demonstration for the IR sensor driver.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pico/stdio.h"
#include "ir_sensor.h"
#include "barcode.h"

#define ADC_PIN_LEFT   26 // Pin for left IR_sensor for analog ouput.
#define ADC_PIN_FRONT  27 // Pin for front IR_sensor for analog output
#define GPIO_PIN_LEFT  15 // Pin for left IR_sensor for digital output
#define GPIO_PIN_FRONT 16 // Pin for front IR_sensor for digital output

int
main (void)
{
    stdio_init_all();
    setup_adc_pins(ADC_PIN_LEFT);
    setup_adc_pins(ADC_PIN_FRONT);
    setup_gpio_pins(GPIO_PIN_LEFT, GPIO_PIN_FRONT);

    barcode_line_buffer_t barcode_line_buffer;
    barcode_clear_line_buffer(&barcode_line_buffer);

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c.
    {
        barcode_line_type_t barcode_line_type = read_barcode();
        // char *barcode_line_str = barcode_line_to_string(barcode_line_type);
        // printf("Barcode line type: %s\n", barcode_line_str);

        // if (BARCODE_LINE_NONE == barcode_line_type)
        // {
        //     barcode_clear_line_buffer(&barcode_line_buffer);
        //     continue;
        // }

        int8_t barcode_read_response = barcode_update_line_buffer(
            &barcode_line_buffer, barcode_line_type);

        if (BARCODE_READ_ERROR == barcode_read_response)
        {
            // printf("Barcode read error.\n");
            barcode_clear_line_buffer(&barcode_line_buffer);
            goto end_barcode_read;
        }

        if (BARCODE_READ_NO_OP == barcode_read_response
            || BARCODE_READ_CONTINUE == barcode_read_response)
        {
            goto end_barcode_read;
        }

        printf("%u\n", barcode_line_type);

        if (BARCODE_READ_SUCCESS == barcode_read_response)
        {
            char *p_barcode_buffer_str
                = barcode_buffer_to_binary_string(&barcode_line_buffer);
            printf("0b%s\n", p_barcode_buffer_str);
            free(p_barcode_buffer_str);
            barcode_char_t barcode_char
                = barcode_get_barcode_char(&barcode_line_buffer);
            char decoded_char = barcode_get_char(barcode_char);
            if ('~' != decoded_char)
            {

                printf("Barcode read: %c\n", decoded_char);
                barcode_clear_line_buffer(&barcode_line_buffer);
                continue;
            }
        }

    end_barcode_read:

        find_wall_directions(GPIO_PIN_LEFT, GPIO_PIN_FRONT);
    }
}

// End of driver/irline/main.c.
