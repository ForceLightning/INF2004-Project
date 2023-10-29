#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "wheel_encoder.h"

#define MOTOR_PIN_CLKWISE 16
#define MOTOR_PIN_ANTICLKWISE 17

#define PWM_PIN 0

#define CYCLE_PULSE 20
#define DISTANCE_PER_PULSE 204.203/20.0

/**
 * @brief Get the time difference in ms
 * 
 * @param current_time 
 * @param prev_time 
 * @return float 
 */
float get_time_diff(uint64_t current_time, uint64_t prev_time){
    return (current_time - prev_time)/1000.0;
}

/**
 * @brief Get the speed in either pulses/second or mm/second
 * 
 * @param time_elapsed 
 * @param is_pulse 
 * @return float 
 */
float get_speed(float time_elapsed, uint is_pulse){
    if(is_pulse){
        return 1000.0/time_elapsed;
    }
    else{
        return (1000.0/time_elapsed) * DISTANCE_PER_PULSE;
    }
}