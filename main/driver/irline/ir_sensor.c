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
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "ir_sensor.h"
#include "barcode.h"

/**
 * @brief Initalises the analog pins for the IR sensor to ADC.
 *
 * @param adc_pin ADC pin for the IR sensor.
 */
void
setup_adc_pin (uint adc_pin)
{
    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(0);
}

/**
 * @brief Initialises the digital pin for the IR sensor.
 *
 * @param gpio_pin GPIO pin for the IR sensor
 */
void
setup_gpio_pin (uint gpio_pin)
{
    gpio_init(gpio_pin);
    gpio_set_dir(gpio_pin, GPIO_IN);
}

/**
 * @brief Detects line
 *
 * @param gpio_pin_in GPIO pin for the IR sensor
 * @return uint16_t
 */
uint16_t
read_line (uint gpio_pin_in)
{
    uint16_t digital_result = gpio_get(gpio_pin_in);
    return digital_result;
}

/**
 * @brief Indicates whether there is a top wall
 *
 * @param flag The indicator for the walls
 */
void
update_top_flag (ir_flags_t *p_flag)
{
    p_flag->top_wall = 1;
}

/**
 * @brief Indicates whether there is a left wall
 *
 * @param flag The indicator for the walls
 */
void
update_left_flag (ir_flags_t *p_flag)
{
    p_flag->left_wall = 1;
}

/**
 * @brief This returns the information of the walls in the node
 *
 * @param gpio_pin_left GPIO pin for the left sensor
 * @param gpio_pin_right GPIO pin for the front sensor
 * @return uint16_t
 */
uint16_t
find_wall_directions (uint gpio_pin_left, uint gpio_pin_right)
{
    uint16_t has_wall = 0;
    if (read_line(gpio_pin_left) == 1)
    {
        has_wall += 8;
    }
    if (read_line(gpio_pin_right) == 1)
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
barcode_line_type_t
read_barcode (void)
{
    uint16_t result = adc_read();

    barcode_line_type_t barcode_type = BARCODE_LINE_NONE;

    if (180 < result && 350 >= result)
    {
        barcode_type = BARCODE_LINE_WHITE_THICK;
    }
    else if (350 < result && 1800 >= result)
    {
        barcode_type = BARCODE_LINE_WHITE_THIN;
    }
    else if (1800 < result && 3600 >= result)
    {
        barcode_type = BARCODE_LINE_BLACK_THIN;
    }
    else if (3600 < result && 4095 >= result)
    {
        barcode_type = BARCODE_LINE_BLACK_THICK;
    }

    return barcode_type;
}
// End of file driver/ir_sensor/ir_sensor.c.