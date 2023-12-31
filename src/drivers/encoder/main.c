/**
 * @file main.c
 * @author Bryan Seah
 * @brief Driver demo code for the wheel encoder.
 * @version 0.1
 * @date 2023-11-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdio.h"
#include "encoder/wheel_encoder.h"

/**
 * @defgroup encoder_demo_constants Encoder Demo Constants
 * @brief Constants for the encoder demo.
 * @{
 */

/** @brief GPIO pin number for clockwise rotation. */
#define MOTOR_PIN_CLKWISE 16
/** @brief GPIO pin number for anticlockwise rotation. */
#define MOTOR_PIN_ANTICLKWISE 17
/** @brief GPIO pin number for PWM output to control motor speed. */
#define PWM_PIN 0
/** @brief PWM clock divider. */
#define PWM_CLKDIV 100
/** @brief PWM wrap value. */
#define PWM_WRAP 62500
/** @brief GP22 for encoder input. */
#define ENCODER_PIN 22
/** @brief Conversion factor from mm to cm. */
#define MM_TO_CM 10.0f

/**
 @} */ // End of encoder_demo_constants group.

// Global variables.
// -----------------------------------------------------------------------------
//

/** @brief Global struct for holding encoder data. */
wheel_encoder_t g_encoder;

// Private function prototypes.
// -----------------------------------------------------------------------------
//
static void init_global_structs(void);
static void encoder_tick_isr(uint gpio, uint32_t events);

int
main ()
{
    // Initialisation.
    //
    stdio_init_all();
    init_global_structs(); // Initialise global struct for encoder data.

    // GPIO setup. Set PWM pin to PWM mode, motor pins to output.
    //
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    gpio_init(MOTOR_PIN_CLKWISE);
    gpio_init(MOTOR_PIN_ANTICLKWISE);

    gpio_set_dir(MOTOR_PIN_CLKWISE, GPIO_OUT);
    gpio_set_dir(MOTOR_PIN_ANTICLKWISE, GPIO_OUT);

    gpio_put(MOTOR_PIN_CLKWISE, 1);
    gpio_put(MOTOR_PIN_ANTICLKWISE, 0);

    // Initialise PWM.
    //
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Get PWM slice number.
    pwm_set_clkdiv(slice_num, PWM_CLKDIV);
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, PWM_WRAP / 2);
    pwm_set_enabled(slice_num, true);

    // Call interrupt on rising edge.
    //
    gpio_set_irq_enabled_with_callback(
        ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_tick_isr);

    for (;;)
    {
        char user_input = getchar();

        switch (user_input)
        {
            case 'f':
                pwm_set_wrap(slice_num, PWM_WRAP);
                pwm_set_chan_level(slice_num, PWM_CHAN_A, PWM_WRAP);
                break;
            case 'r': // Redundant?
                pwm_set_wrap(slice_num, PWM_WRAP);
                pwm_set_chan_level(slice_num, PWM_CHAN_A, PWM_WRAP / 2);
                break;
            default:
                break;
        }
    }
}

// Private functions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Sets up global struct(s).
 */
static void
init_global_structs (void)
{
    g_encoder.prev_time          = 0;
    g_encoder.pulse_count        = 0;
    g_encoder.distance_traversed = 0.0f;
}

/**
 * @brief Interrupt callback function on rising edge
 *
 * @param[in] gpio GPIO pin number.
 * @param[in] events Event mask. @see gpio_irq_level.
 */
static void
encoder_tick_isr (uint gpio, uint32_t events)
{
    uint64_t current_time = time_us_64();
    g_encoder.pulse_count++;

    // Calculate time elapsed since last interrupt speed
    float time_elapsed
        = wheel_enc_get_time_diff(current_time, g_encoder.prev_time);

    // Calculate current speed based on elapsed time
    float current_speed_pulse = wheel_enc_get_speed(time_elapsed, true);
    float current_speed_mm    = wheel_enc_get_speed(time_elapsed, false);

    printf("\nCurrent speed: %f pulses/second", current_speed_pulse);
    printf("\tCurrent speed: %f cm/second", current_speed_mm / MM_TO_CM);

    // Update distance traversed
    g_encoder.distance_traversed += WHEEL_ENC_DIST_PER_PULSE;
    printf("\tDistance traversed: %fcm",
           g_encoder.distance_traversed / MM_TO_CM);

    // Reset pulse count when one full cycle is completed
    if (g_encoder.pulse_count == WHEEL_ENC_CYCLE_PULSE)
    {
        g_encoder.pulse_count = 0;
    }

    g_encoder.prev_time = current_time;
}

// End of file drivers/encoder/main.c.
