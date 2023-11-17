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
 * @brief Initialises the digital pin for the IR sensors.
 *
 * @param gpioPinOne GPIO pin for the first IR sensor
 * @param gpioPinTwo GPIO pin for the second IR sensor
 */
void 
setup_gpio_pins(uint gpioPinOne, uint gpioPinTwo)
{
    gpio_init(gpioPinOne);
    gpio_init(gpioPinTwo);
    gpio_set_dir(gpioPinOne, GPIO_IN);
    gpio_set_dir(gpioPinTwo, GPIO_IN);
}

/**
 * @brief Detects line
 * 
 * @param gpioPinIn GPIO pin for the IR sensor
 * @return uint16_t 
 */
uint16_t read_line(uint gpioPinIn)
{
    uint16_t digital_result = gpio_get(gpioPinIn);
    return digital_result;
}

/**
 * @brief Indicates whether there is a top wall
 * 
 * @param flag The indicator for the walls 
 */
void update_top_flag(struct flags * flag)
{
    flag->top_wall = 1;
}

/**
 * @brief Indicates whether there is a left wall
 * 
 * @param flag The indicator for the walls 
 */
void update_left_flag(struct flags * flag)
{
    flag->left_wall = 1;
}

/**
 * @brief This returns the information of the walls in the node 
 * 
 * @param gpioPinLeft GPIO pin for the left sensor
 * @param gpioPinFront GPIO pin for the front sensor
 * @return uint16_t 
 */
uint16_t 
find_wall_directions(uint gpioPinLeft, uint gpioPinFront)
{
    uint16_t has_wall = 0;
    if (read_line(gpioPinLeft) == 1)
    {
        has_wall += 8;
    }
    if (read_line(gpioPinFront) == 1)
    {
        has_wall += 1;
        
    }
    return has_wall;
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