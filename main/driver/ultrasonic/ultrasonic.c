/**
 * @file ultrasonic.c
 * @author Ang Jia Yu
 * @brief Driver code for the ultrasonic sensor.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "pico/time.h"
#include "pico/types.h"
#include "pico/platform.h"
#include "hardware/gpio.h"

#include "ultrasonic.h"

ultrasonic_data_t g_ultrasonic_data;

/**
 * @brief This function is the interrupt service routine when receiving a
 ultrasonic pulse.
 *
 * @param gpio GPIO pin that triggered the interrupt.
 * @param events The type of event that triggered the interrupt.

 */
static void
ultrasonic_pulse_isr (__unused uint gpio, uint32_t events)
{
    // Check if the pulse is starting or ending.
    //
    if (GPIO_IRQ_EDGE_RISE == events)
    {
        g_ultrasonic_data.g_start_time = get_absolute_time();
        g_ultrasonic_data.g_width++;
        if (ULTRASONIC_TIMEOUT < g_ultrasonic_data.g_width)
        {
            g_ultrasonic_data.g_width = 0;
        }
    }
    else if (GPIO_IRQ_EDGE_FALL == events)
    {
        g_ultrasonic_data.g_end_time    = get_absolute_time();
        g_ultrasonic_data.g_pulse_width = absolute_time_diff_us(g_ultrasonic_data.g_start_time, g_ultrasonic_data.g_end_time);
    }
}

/**
 * @brief Initalises the pins for the ultrasonic sensor.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 *
 * @par Initialises the trigger and echo pins, and sets a ISR on the echo pin to
 * measure the pulse width.
 */
void
init_ultrasonic_pins (uint trig_pin, uint echo_pin)
{
    gpio_init(trig_pin);
    gpio_init(echo_pin);
    gpio_set_dir(trig_pin, GPIO_OUT);
    gpio_set_dir(echo_pin, GPIO_IN);
    printf("init ultrasonic pins\n");
    gpio_set_irq_enabled_with_callback(echo_pin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true,
                                       &ultrasonic_pulse_isr);
}

/**
 * @brief Get the pulse length in cycles.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The pulse length in cycles.
 */
uint64_t
get_pulse (uint trig_pin, uint echo_pin)
{
    gpio_put(trig_pin, 1);
    sleep_us(ULTRASONIC_TRIG_PULSE_US);
    gpio_put(trig_pin, 0);
    g_ultrasonic_data.g_width = 0;
    return g_ultrasonic_data.g_pulse_width;
}

/**
 * @brief Get the distance to an object in centimetres.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The distance to an object in centimetres.
 */
uint64_t
get_cm (uint trig_pin, uint echo_pin)
{
    uint64_t pulse_length = get_pulse(trig_pin, echo_pin);
    // printf("pulse length: %llu\n", pulse_length);

    return pulse_length / 29 / 2;
}

/**
 * @brief Get the distance to an object in inches.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The distance to an object in inches.
 */
uint64_t
get_inches (uint trig_pin, uint echo_pin)
{
    uint64_t pulse_length = get_pulse(trig_pin, echo_pin);
    // print("pulse length: %llu\n", pulse_length);
    return (uint64_t)pulse_length / 74.f / 2.f;
}

// End of file driver/ultrasonic/ultrasonic.c.