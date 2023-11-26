/**
 * @file ir_sensor.h
 * @author Ang Jia Yu
 * @brief Header file for the IR sensor driver.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef IR_SENSOR_H // Include guard.
#define IR_SENSOR_H
#include "pico/types.h"
#include "barcode.h"

/**
 * @typedef ir_flags_t
 * @brief Indicates whether the IR line sensor detects walls on the top and left
 * sides.
 */
typedef struct ir_flags
{
    bool top_wall;  ///< Indicates whether there is a top wall.
    bool left_wall; ///< Indicates whether there is a left wall.
} ir_flags_t;

// Function prototypes.
//
void                ir_setup_adc_pin(uint adc_pin);
void                ir_setup_gpio_pin(uint gpio_pin);
barcode_line_type_t ir_read_barcode(void);
uint16_t            ir_read_line(uint gpio_pin_in);
void                ir_update_top_flag(ir_flags_t *flag);
void                ir_update_left_flag(ir_flags_t *flag);
uint16_t ir_find_wall_directions(uint gpio_pin_left, uint gpio_pin_right);

#endif // IR_SENSOR_H

// End of file driver/ir_sensor/ir_sensor.h.
