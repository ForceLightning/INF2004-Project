/**
 * @file ir_sensor.c
 * @author Ang Jia Yu
 * @brief Driver code for the IR sensor driver.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include "hardware/adc.h"

/**
 * @brief Initalises the pins for the ADC.
 *
 * @param adc_pin ADC pin for the IR sensor
 */
void
setup_adc_pins (uint adc_pin)
{
    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(0);
}

/**
 * @brief This function reads the barcode and determines the colour and the
 * thickness.
 *
 */
void
read_barcode ()
{
    uint16_t result = adc_read();

    if (200 < result && 500 >= result)
    {
        printf("THICK WHITE BARCODE\n");
    }
    else if (500 < result && 1800 >= result)
    {
        printf("THIN WHITE BARCODE\n");
    }
    else if (1800 < result && 3300 >= result)
    {
        printf("THIN BLACK BARCODE\n");
    }
    else if (3300 < result && 4095 >= result)
    {
        printf("THICK BLACK BARCODE\n");
    }
}