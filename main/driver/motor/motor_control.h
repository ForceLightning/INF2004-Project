/**
 * @file motor_control.h
 * @author Bryan Seah
 * @brief 
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include "pico/stdlib.h"

// Definitions
// 
#define MOTOR_PWM_CLKDIV 100   // PWM clock divider.
#define MOTOR_PWM_WRAP   62500 // PWM wrap value.

typedef struct motor_pins {
    uint pwm_gpio_a;
    uint pwm_gpio_b;
    uint left_clkwise;
    uint left_anticlkwise;
    uint right_clkwise;
    uint right_anticlkwise;
} motor_pins_t;

// Function prototypes
//
void start_motor(uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio);
void update_pwm(uint pwm_gpio, uint pwm_wrap, float duty_cycle);
void update_direction(uint clkwise_gpio,
                      uint anti_clkwise_gpio,
                      uint clkwise,
                      uint anti_clkwise);
void move_forward(motor_pins_t motor_data);
void stop(motor_pins_t motor_data);
void reverse(motor_pins_t motor_data);
void turn_left(motor_pins_t motor_data, bool reverse_turn);
void turn_right(motor_pins_t motor_data, bool reverse_turn);

#endif // MOTOR_CONTROL_H

// End of file motor_control.h
