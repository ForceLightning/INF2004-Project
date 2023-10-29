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

#define ADC_PIN 26 // Pin for IR_sensor for analog ouput.

int
main (void)
{
    stdio_init_all();
    setup_adc_pins(ADC_PIN);

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c.
    {
        read_barcode();
    }

}

// End of driver/irline/main.c.
