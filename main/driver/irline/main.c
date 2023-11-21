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

#include "pico/stdio.h"
#include "ir_sensor.h"

#define ADC_PIN_LEFT 26 // Pin for left IR_sensor for analog ouput.
#define ADC_PIN_FRONT 27 // Pin for front IR_sensor for analog output
#define GPIO_PIN_LEFT 15 // Pin for left IR_sensor for digital output
#define GPIO_PIN_FRONT 16 // Pin for front IR_sensor for digital output


int
main (void)
{
    stdio_init_all();
    setup_adc_pins(ADC_PIN_LEFT);
    setup_adc_pins(ADC_PIN_FRONT);
    setup_gpio_pins(GPIO_PIN_LEFT, GPIO_PIN_FRONT);


    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c.
    {
        // read_barcode();
        find_wall_directions(GPIO_PIN_LEFT, GPIO_PIN_FRONT);
    }

}

// End of driver/irline/main.c.
