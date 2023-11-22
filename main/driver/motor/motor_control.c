/**
 * @file motor_control.c
 * @author Bryan Seah
 * @brief 
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 */
#include <stdio.h>
#include <stdbool.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pico/types.h"
#include "motor_control.h"

float ratio = 1.05f;

/**
 * @brief Default initialization of a single motor.
 *
 * @par The function initializes the PWM pin and the two GPIO pins for the
 * motor. It also sets the clock divider to 100 and enables the PWM slice.
 *
 * @param clkwise_gpio GPIO pin number for clockwise rotation.
 * @param anti_clkwise_gpio GPIO pin number for anticlockwise rotation.
 * @param pwm_gpio GPIO pin number for PWM output to control motor speed.
 */
void
start_motor (uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio)
{
    gpio_set_function(pwm_gpio, GPIO_FUNC_PWM);

    gpio_init(clkwise_gpio);
    gpio_init(anti_clkwise_gpio);

    gpio_set_dir(clkwise_gpio, GPIO_OUT);
    gpio_set_dir(anti_clkwise_gpio, GPIO_OUT);

    pwm_set_clkdiv(pwm_gpio_to_slice_num(pwm_gpio), MOTOR_PWM_CLKDIV);
    pwm_set_enabled(pwm_gpio_to_slice_num(pwm_gpio), true);
}

/**
 * @brief A function to update the PWM duty cycle and wrap value.
 *
 * @par This function updates the PWM duty cycle and wrap value of a PWM slice.
 * This is used to update / modify the speed of the motor's rotation.
 *
 * @param pwm_gpio The GPIO pin number for the PWM output.
 * @param pwm_wrap The wrap value of the PWM slice.
 * @param duty_cycle The duty cycle of the PWM slice.
 */
void
update_pwm (uint pwm_gpio, uint pwm_wrap, float duty_cycle)
{
    // Make sure that the duty cycle is in (0, 1)
    if (1.0f < duty_cycle)
    {
        duty_cycle = 1.0f;
    }
    else if (0.0f > duty_cycle)
    {
        duty_cycle = 0.0f;
    }

    pwm_set_wrap(pwm_gpio_to_slice_num(pwm_gpio), pwm_wrap);
    pwm_set_chan_level(
        pwm_gpio_to_slice_num(pwm_gpio),
        pwm_gpio_to_channel(pwm_gpio),
        (pwm_wrap * duty_cycle)); // Conversion from float to uint is safe
                                  // because duty_cycle is in (0, 1).
}

/**
 * @brief A function to update the direction of the motor's rotation.
 *
 * @param clkwise_gpio The GPIO pin number for clockwise rotation.
 * @param anti_clkwise_gpio The GPIO pin number for anticlockwise rotation.
 * @param clkwise The digital value to set for clockwise rotation.
 * @param anti_clkwise The digital value to set for anticlockwise rotation.
 */
void
update_direction (uint clkwise_gpio,
                  uint anti_clkwise_gpio,
                  uint clkwise,
                  uint anti_clkwise)
{
    gpio_put(clkwise_gpio, clkwise);
    gpio_put(anti_clkwise_gpio, anti_clkwise);
}

/**
 * @brief A function to move the car forward
 *
 * This function moves the car forward by setting the duty cycle of both motors
 * to be equal, as well as setting a clockwise rotation for both motors.
 *
 * @param motor_data The structure containing the GPIO pin numbers for the
 * motors.
 */
void
move_forward ()
{
    printf("Moving forward\n");
    update_pwm(PWM_PIN_LEFT, MOTOR_PWM_WRAP, 0.5f);
    update_pwm(PWM_PIN_RIGHT, MOTOR_PWM_WRAP, 0.5f);
    update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 1, 0);
    update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 1, 0);
}

/**
 * @brief A function to stop the car
 *
 * This function works by setting the duty cycle of both motors to 0.
 *
 * @param motor_data The structure containing the GPIO pin numbers for the
 * motors.
 */
void
stop ()
{
    update_pwm(PWM_PIN_LEFT, MOTOR_PWM_WRAP, 0.0f);
    update_pwm(PWM_PIN_RIGHT, MOTOR_PWM_WRAP, 0.0f);
}

/**
 * @brief A function to reverse the car
 *
 * This function reverses the car by setting the duty cycle of both motors
 * to be equal, as well as setting an anti-clockwise rotation for both motors.
 *
 * @param motor_data The structure containing the GPIO pin numbers for the
 * motors.
 */
void
reverse ()
{
    update_pwm(PWM_PIN_LEFT, MOTOR_PWM_WRAP, 0.5f);
    update_pwm(PWM_PIN_RIGHT, MOTOR_PWM_WRAP, 0.5f);
    update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 0, 1);
    update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 0, 1);
}

/**
 * @brief A function to turn the car left
 *
 * This function turns the car left by setting the duty cycle of the left motor
 * to be lower than the right motor, which enables the car to turn left. In
 * addition, the left motor is set to rotate in the opposite direction of the
 * right motor.
 *
 * @param motor_data The structure containing the GPIO pin numbers for the
 * motors.
 * @param reverse_turn A boolean value to indicate that the car is turning in
 * reverse.
 */
void
turn_left (bool reverse_turn)
{
    update_pwm(PWM_PIN_LEFT, MOTOR_PWM_WRAP, 0.2f);
    update_pwm(PWM_PIN_RIGHT, MOTOR_PWM_WRAP, 0.5f);

    if (reverse_turn)
    {
        update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 1, 0);
        update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 0, 1);
    }
    else
    {
        update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 0, 1);
        update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 1, 0);
    }
}

/**
 * @brief A function to turn the car right
 *
 * This function turns the car right by setting the duty cycle of the right
 * motor to be lower than the left motor, which enables the car to turn left.
 * In addition, the right motor is set to rotate in the opposite direction of
 * the right motor.
 *
 * @param motor_data The structure containing the GPIO pin numbers for the
 * motors.
 * @param reverse_turn A boolean value to indicate that the car is turning in
 * reverse.
 */
void
turn_right (bool reverse_turn)
{
    update_pwm(PWM_PIN_LEFT, MOTOR_PWM_WRAP, 0.5f);
    update_pwm(PWM_PIN_RIGHT, MOTOR_PWM_WRAP, 0.2f);

    if (reverse_turn)
    {
        update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 0, 1);
        update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 1, 0);
    }
    else
    {
        update_direction(LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, 1, 0);
        update_direction(RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, 0, 1);
    }
}

void update_ratio(float new_ratio){
    ratio = new_ratio;
}
// End of file motor_control.c