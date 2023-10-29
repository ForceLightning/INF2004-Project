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
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define ADC_PIN 26 // Pin for IR_sensor for analog ouput

/**
 * @brief Initalises the pins for the ADC.
 *
 * @param adcPin ADC pin for the IR sensor
 */
void setupADCPins(uint adcPin)
{
    adc_init();
    adc_gpio_init(adcPin);
    adc_select_input(0);
}

/**
 * @brief This function reads the barcode and determines the colour and the thickness
 * 
 */
void readBarcode()
{
    uint16_t result = adc_read();

    if (result >= 200 && result <= 500)
    {
        printf("THICK WHITE BARCODE\n");
    }
    else if (result >= 501 && result <= 1800)
    {
        printf("THIN WHITE BARCODE\n");
    }
    else if (result >= 1801 && result <= 3300)
    {
        printf("THIN BLACK BARCODE\n");
    }
    else if (result >= 3301 && result <= 4000)
    {
        printf("THICK BLACK BARCODE\n");
    }
}