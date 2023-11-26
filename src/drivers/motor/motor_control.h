/**
 * @file motor_control.h
 * @author Bryan Seah
 * @brief Header file for motor control drivers.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include <stdbool.h>
#include "pico/stdlib.h"
#include "pico/types.h"

// Definitions
// --------------------------------------------------------------------------------------------
//
#define MOTOR_PWM_CLKDIV 100   // PWM clock divider.
#define MOTOR_PWM_WRAP   62500 // PWM wrap value.

// Definitions for the motor pins.
//
#define MOTOR_LEFT_PIN_CLKWISE     16 // GPIO pin for left motor clockwise.
#define MOTOR_LEFT_PIN_ANTICLKWISE 17 // GPIO pin for left motor anti-clockwise.
#define MOTOR_RIGHT_PIN_CLKWISE    15 // GPIO pin for right motor clockwise.
#define MOTOR_RIGHT_PIN_ANTICLKWISE \
    14                         // GPIO pin for right motor anti-clockwise.
#define MOTOR_PWM_PIN_LEFT  10 // GPIO pin for left motor PWM.
#define MOTOR_PWM_PIN_RIGHT 11 // GPIO pin for right motor PWM.
#define MOTOR_DEFAULT_DIFF_RATIO \
    1.05f // Default ratio for motor speed difference.

// Type definitions
// --------------------------------------------------------------------------------------------
//

/**
 * @typedef motor_pins_t
 * @brief Contains the GPIO pins for the motor.
 *
 */
typedef struct motor_pins
{
    uint pwm_gpio_a;        ///< GPIO pin for PWM A.
    uint pwm_gpio_b;        ///< GPIO pin for PWM B.
    uint left_clkwise;      ///< GPIO pin for left motor clockwise.
    uint left_anticlkwise;  ///< GPIO pin for left motor anti-clockwise.
    uint right_clkwise;     ///< GPIO pin for right motor clockwise.
    uint right_anticlkwise; ///< GPIO pin for right motor anti-clockwise.
} motor_pins_t;

// Function prototypes
//
void motor_start(uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio);
void motor_update_pwm(uint pwm_gpio, uint pwm_wrap, float duty_cycle);
void motor_update_direction(uint clkwise_gpio,
                            uint anti_clkwise_gpio,
                            uint clkwise,
                            uint anti_clkwise);
void motor_move_forward(void);
void motor_stop(void);
void motor_reverse(void);
void motor_turn_left(bool reverse_turn);
void motor_turn_right(bool reverse_turn);

void motor_update_ratio(float new_ratio);

#endif // MOTOR_CONTROL_H

// End of file motor_control.h