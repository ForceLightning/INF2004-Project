#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include "pico/stdlib.h"

// Function prototypes
void start_motor(uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio);
void update_pwm(uint pwm_gpio, uint pwm_wrap, float duty_cycle);
void update_direction(uint clkwise_gpio, uint anti_clkwise_gpio, uint clkwise, uint anti_clkwise);
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
               uint reverse_turn);
void turn_right(uint pwm_gpio_a, 
                uint pwm_gpio_b, 
                uint left_clkwise, 
                uint left_anticlkwise, 
                uint right_clkwise, 
                uint right_anticlkwise,
                uint reverse_turn);

#endif // MOTOR_CONTROL_H