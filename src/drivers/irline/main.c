/**
 * @file main.c
 * @author Ang Jia Yu and Christopher Kok
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
#include "irline/ir_sensor.h"
#include "irline/barcode.h"

/**
 * @defgroup ir_sensor_demo IR Sensor Demo Constants
 * @brief Constants for the IR sensor demo.
 * @{
 */

/** @brief Pin for left IR sensor's analogue output. */
#define ADC_PIN_LEFT 26
/** @brief Pin for front IR sensor's analogue output. */
#define ADC_PIN_FRONT 27
/** @brief Pin for left IR sensor's digital output. */
#define GPIO_PIN_LEFT 15
/** @brief Pin for front IR sensor's digital output. */
#define GPIO_PIN_FRONT 16

/**
 @} */ // End of ir_sensor_demo group.

int
main (void)
{
    stdio_init_all();
    scanf("Press enter to start barcode read.\n");
    printf("Starting barcode read.\n");
    ir_setup_adc_pin(ADC_PIN_LEFT);
    ir_setup_adc_pin(ADC_PIN_FRONT);
    ir_setup_gpio_pin(GPIO_PIN_LEFT);
    ir_setup_gpio_pin(GPIO_PIN_FRONT);

    barcode_line_buffer_t barcode_line_buffer;
    barcode_clear_line_buffer(&barcode_line_buffer);

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c.
    {
        barcode_line_type_t barcode_line_type = ir_read_barcode();

        // Check whether the barcode line is valid.
        //
        int8_t barcode_read_response = barcode_update_line_buffer(
            &barcode_line_buffer, barcode_line_type);

        if (BARCODE_READ_ERROR == barcode_read_response)
        {
            DEBUG_PRINT("DEBUG: Barcode read error.\n");
            barcode_clear_line_buffer(&barcode_line_buffer);
            goto end_barcode_read;
        }
        if (1 == BARCODE_DEBUG_VERBOSE)
        {
            char *p_barcode_buffer_str
                = barcode_buffer_to_binary_string(&barcode_line_buffer);
            if (NULL != p_barcode_buffer_str)
            {
                DEBUG_PRINT("DEBUG: %s\n", p_barcode_buffer_str);
                free(p_barcode_buffer_str);
            }
            else
            {
                DEBUG_PRINT("DEBUG: Barcode buffer string is NULL.\n");
            }
        }

        // If the barcode line buffer is not full, continue reading.
        //
        if (BARCODE_READ_NO_OP == barcode_read_response
            || BARCODE_READ_CONTINUE == barcode_read_response)
        {
            // DEBUG_PRINT("DEBUG: No-op\n");
            goto end_barcode_read;
        }
        // Otherwise, decode the barcode character.
        //
        if (BARCODE_READ_SUCCESS == barcode_read_response)
        {
            barcode_char_t barcode_char
                = barcode_decode_barcode_char(&barcode_line_buffer);
            char decoded_char = barcode_get_char(barcode_char);
            if ('~' != decoded_char)
            {
                printf("Decoded character: %c\n", decoded_char);
            }
            barcode_clear_line_buffer(&barcode_line_buffer);
        }

    end_barcode_read:

        ir_find_wall_directions(GPIO_PIN_LEFT, GPIO_PIN_FRONT);
    }
}

// End of driver/irline/main.c.
