#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

/**
 * @brief Default initialization of a single motor
 * 
 * The function initializes the PWM pin and the two GPIO pins for the motor.
 * It also sets the clock divider to 100 and enables the PWM slice.
 * 
 * @param clkwise_gpio 
 * @param anti_clkwise_gpio 
 * @param pwm_gpio 
 */
void start_motor(uint clkwise_gpio, uint anti_clkwise_gpio, uint pwm_gpio){
    gpio_set_function(pwm_gpio, GPIO_FUNC_PWM);

    gpio_init(clkwise_gpio);
    gpio_init(anti_clkwise_gpio);

    gpio_set_dir(clkwise_gpio, GPIO_OUT);
    gpio_set_dir(anti_clkwise_gpio, GPIO_OUT);

    pwm_set_clkdiv(pwm_gpio_to_slice_num(pwm_gpio), 100);
    pwm_set_enabled(pwm_gpio_to_slice_num(pwm_gpio), true);
}

/**
 * @brief A function to update the PWM duty cycle and wrap value
 * 
 * This function updates the PWM duty cycle and wrap value of a PWM slice.
 * This is used to update / modify the speed of the motor's rotation.
 * 
 * @param pwm_gpio 
 * @param pwm_wrap 
 * @param duty_cycle 
 */
void update_pwm(uint pwm_gpio, uint pwm_wrap, float duty_cycle){
    pwm_set_wrap(pwm_gpio_to_slice_num(pwm_gpio), pwm_wrap);
    pwm_set_chan_level(pwm_gpio_to_slice_num(pwm_gpio), pwm_gpio_to_channel(pwm_gpio), (pwm_wrap * duty_cycle));
}

/**
 * @brief A function to update the direction of the motor's rotation
 * 
 * @param clkwise_gpio 
 * @param anti_clkwise_gpio 
 * @param clkwise 
 * @param anti_clkwise 
 */
void update_direction(uint clkwise_gpio, uint anti_clkwise_gpio, uint clkwise, uint anti_clkwise){
    gpio_put(clkwise_gpio, clkwise);
    gpio_put(anti_clkwise_gpio, anti_clkwise);
}

/**
 * @brief A function to move the car forward
 * 
 * This function moves the car forward by setting the duty cycle of both motors
 * to be equal, as well as setting a clockwise rotation for both motors.
 * 
 * @param pwm_gpio_a 
 * @param pwm_gpio_b 
 * @param left_clkwise 
 * @param left_anticlkwise 
 * @param right_clkwise 
 * @param right_anticlkwise 
 */
void move_forward(uint pwm_gpio_a, 
                  uint pwm_gpio_b, 
                  uint left_clkwise, 
                  uint left_anticlkwise, 
                  uint right_clkwise, 
                  uint right_anticlkwise){
    update_pwm(pwm_gpio_a, 62500, 0.5);
    update_pwm(pwm_gpio_b, 62500, 0.5);
    update_direction(left_clkwise, left_anticlkwise, 1, 0);
    update_direction(right_clkwise, right_anticlkwise, 1, 0);
}

/**
 * @brief A function to stop the car
 * 
 * This function works by setting the duty cycle of both motors to 0.
 * 
 * @param pwm_gpio_a 
 * @param pwm_gpio_b
 */
void stop(uint pwm_gpio_a, uint pwm_gpio_b){
    update_pwm(pwm_gpio_a, 62500, 0);
    update_pwm(pwm_gpio_b, 62500, 0);
}

/**
 * @brief A function to reverse the car
 * 
 * This function reverses the car by setting the duty cycle of both motors
 * to be equal, as well as setting an anti-clockwise rotation for both motors.
 * 
 * @param pwm_gpio_a 
 * @param pwm_gpio_b 
 * @param left_clkwise 
 * @param left_anticlkwise 
 * @param right_clkwise 
 * @param right_anticlkwise 
 */
void reverse(uint pwm_gpio_a, 
             uint pwm_gpio_b, 
             uint left_clkwise, 
             uint left_anticlkwise, 
             uint right_clkwise, 
             uint right_anticlkwise){
    update_pwm(pwm_gpio_a, 62500, 0.5);
    update_pwm(pwm_gpio_b, 62500, 0.5);
    update_direction(left_clkwise, left_anticlkwise, 0, 1);
    update_direction(right_clkwise, right_anticlkwise, 0, 1);
}

/**
 * @brief A function to turn the car left
 * 
 * This function turns the car left by setting the duty cycle of the left motor
 * to be lower than the right motor, which enables the car to turn left. In 
 * addition, the left motor is set to rotate in the opposite direction of the
 * right motor.
 * 
 * @param pwm_gpio_a 
 * @param pwm_gpio_b 
 * @param left_clkwise 
 * @param left_anticlkwise 
 * @param right_clkwise 
 * @param right_anticlkwise 
 * @param reverse_turn 
 */
void turn_left(uint pwm_gpio_a, 
               uint pwm_gpio_b, 
               uint left_clkwise, 
               uint left_anticlkwise, 
               uint right_clkwise, 
               uint right_anticlkwise,
               uint reverse_turn){
    update_pwm(pwm_gpio_a, 62500, 0.2);
    update_pwm(pwm_gpio_b, 62500, 0.5);

    if(reverse_turn){
        update_direction(left_clkwise, left_anticlkwise, 1, 0);
        update_direction(right_clkwise, right_anticlkwise, 0, 1);
    } else {
        update_direction(left_clkwise, left_anticlkwise, 0, 1);
        update_direction(right_clkwise, right_anticlkwise, 1, 0);
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
 * @param pwm_gpio_a 
 * @param pwm_gpio_b 
 * @param left_clkwise 
 * @param left_anticlkwise 
 * @param right_clkwise 
 * @param right_anticlkwise 
 * @param reverse_turn 
 */
void turn_right(uint pwm_gpio_a, 
                uint pwm_gpio_b, 
                uint left_clkwise, 
                uint left_anticlkwise, 
                uint right_clkwise, 
                uint right_anticlkwise,
                uint reverse_turn){
    update_pwm(pwm_gpio_a, 62500, 0.5);
    update_pwm(pwm_gpio_b, 62500, 0.2);

    if(reverse_turn){
        update_direction(left_clkwise, left_anticlkwise, 0, 1);
        update_direction(right_clkwise, right_anticlkwise, 1, 0);
    } else {
        update_direction(left_clkwise, left_anticlkwise, 1, 0);
        update_direction(right_clkwise, right_anticlkwise, 0, 1);
    }
}