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

typedef struct ir_flags
{
    bool top_wall;
    bool left_wall;
} ir_flags_t;

// Function prototypes.
//
void                setup_adc_pin(uint adc_pin);
void                setup_gpio_pin(uint gpio_pin);
barcode_line_type_t read_barcode(void);
uint16_t            read_line(uint gpioPinIn);
void                update_top_flag(ir_flags_t *flag);
void                update_left_flag(ir_flags_t *flag);
uint16_t            find_wall_directions(uint gpioPinLeft, uint gpioPinFront);

#endif // IR_SENSOR_H

// End of file driver/ir_sensor/ir_sensor.h.
