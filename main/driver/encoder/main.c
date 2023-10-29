
#include "wheel_encoder.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#define MOTOR_PIN_CLKWISE 16
#define MOTOR_PIN_ANTICLKWISE 17

#define PWM_PIN 0

#define CYCLE_PULSE 20
#define DISTANCE_PER_PULSE 204.203/20.0

uint64_t prev_time = 0;
uint pulse_count = 0;
float distance_traversed = 0.0;

/**
 * @brief Interrupt callback function on rising edge
 * 
 * @param gpio 
 * @param events 
 */
void encoder_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();
    pulse_count++;

    // Calculate time elapsed since last interrupt speed
    float time_elapsed = get_time_diff(current_time, prev_time);

    // Calculate current speed based on elapsed time
    float current_speed_pulse = get_speed(time_elapsed, 1);
    float current_speed_mm = get_speed(time_elapsed, 0);

    printf("\nCurrent speed: %f pulses/second", current_speed_pulse);
    printf("\tCurrent speed: %f cm/second", current_speed_mm/10.0);

    // Update distance traversed
    distance_traversed += DISTANCE_PER_PULSE;
    printf("\tDistance traversed: %fcm", distance_traversed/10.0);

    // Reset pulse count when one full cycle is completed
    if(pulse_count == CYCLE_PULSE){
        pulse_count = 0;
    }

    prev_time = current_time;
}

int main() {
    stdio_init_all();

    // Set up PWM on motor pin
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    gpio_init(MOTOR_PIN_CLKWISE);
    gpio_init(MOTOR_PIN_ANTICLKWISE);

    gpio_set_dir(MOTOR_PIN_CLKWISE, GPIO_OUT);
    gpio_set_dir(MOTOR_PIN_ANTICLKWISE, GPIO_OUT);

    gpio_put(MOTOR_PIN_CLKWISE, 1);
    gpio_put(MOTOR_PIN_ANTICLKWISE, 0);

    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    pwm_set_clkdiv(slice_num, 100);
    pwm_set_wrap(slice_num, 62500);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 62500/2);
    pwm_set_enabled(slice_num, true);

    // Call interrupt on rising edge
    gpio_set_irq_enabled_with_callback(22, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);

    while (1){
        char c = getchar();

        switch(c){
            case 'f':
                pwm_set_wrap(slice_num, 62500);
                pwm_set_chan_level(slice_num, PWM_CHAN_A, 62500);
                break;
            case 'r':
                pwm_set_wrap(slice_num, 62500);
                pwm_set_chan_level(slice_num, PWM_CHAN_A, 62500/2);
                break;
            default:
                pwm_set_wrap(slice_num, 62500);
                pwm_set_chan_level(slice_num, PWM_CHAN_A, 62500/2);
                break;
        }
    };
}
