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

#ifndef IR_SENSOR_H
#define IR_SENSOR_H
#include "pico/types.h"
struct flags{
    bool top_wall;
    bool left_wall;
};

// Function prototypes.
//
void setup_adc_pins(uint adc_pin);
void setup_gpio_pins(uint gpioPinOne, uint gpioPinTwo);
void read_barcode();
uint16_t read_line(uint gpioPinIn);
void update_top_flag(struct flags * flag);
void update_left_flag(struct flags * flag);
uint16_t find_wall_directions(uint gpioPinLeft, uint gpioPinFront);

#endif // IR_SENSOR_H

// End of file driver/ir_sensor/ir_sensor.h.
