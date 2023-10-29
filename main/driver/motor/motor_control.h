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

// Function prototypes
//
void start_motor(uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio);
void update_pwm(uint pwm_gpio, uint pwm_wrap, float duty_cycle);
void update_direction(uint clkwise_gpio,
                      uint anti_clkwise_gpio,
                      uint clkwise,
                      uint anti_clkwise);
void move_forward(uint pwm_gpio_a,
                  uint pwm_gpio_b,
                  uint left_clkwise,
                  uint left_anticlkwise,
                  uint right_clkwise,
                  uint right_anticlkwise);
void stop(uint pwm_gpio_a, uint pwm_gpio_b);
void reverse(uint pwm_gpio_a,
             uint pwm_gpio_b,
             uint left_clkwise,
             uint left_anticlkwise,
             uint right_clkwise,
             uint right_anticlkwise);
void turn_left(uint pwm_gpio_a,
               uint pwm_gpio_b,
               uint left_clkwise,
               uint left_anticlkwise,
               uint right_clkwise,
               uint right_anticlkwise,
               bool reverse_turn);
void turn_right(uint pwm_gpio_a,
                uint pwm_gpio_b,
                uint left_clkwise,
                uint left_anticlkwise,
                uint right_clkwise,
                uint right_anticlkwise,
                bool reverse_turn);

#endif // MOTOR_CONTROL_H

// End of file motor_control.h
