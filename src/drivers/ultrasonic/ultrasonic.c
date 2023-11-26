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
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "pico/time.h"
#include "pico/types.h"
#include "hardware/gpio.h"

#include "ultrasonic.h"

// Global variables.
// -----------------------------------------------------------------------------
//
static absolute_time_t g_start_time;  // Start time of the pulse.
static absolute_time_t g_end_time;    // End time of the pulse.
static uint64_t        g_pulse_width; // Pulse width in us.
static uint64_t        g_width;       // Pulse width in cycles.

// Private function prototypes.
// -----------------------------------------------------------------------------
//
static void     ultrasonic_pulse_isr(uint gpio, uint32_t events);
static uint64_t ultrasonic_get_pulse(uint trig_pin, uint echo_pin);

// Public functions.
// -----------------------------------------------------------------------------
//

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
ultrasonic_init_pins (uint trig_pin, uint echo_pin)
{
    gpio_init(trig_pin);
    gpio_init(echo_pin);
    gpio_set_dir(trig_pin, GPIO_OUT);
    gpio_set_dir(echo_pin, GPIO_IN);
    gpio_set_irq_enabled_with_callback(echo_pin,
                                       GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                       true,
                                       &ultrasonic_pulse_isr);
}

/**
 * @brief Get the distance to an object in centimetres.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The distance to an object in centimetres.
 */
uint64_t
ultrasonic_get_cm (uint trig_pin, uint echo_pin)
{
    uint64_t pulse_length = ultrasonic_get_pulse(trig_pin, echo_pin);
    return (uint64_t)ULTRASONIC_PULSE_TO_CM(pulse_length);
}

/**
 * @brief Get the distance to an object in inches.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The distance to an object in inches.
 */
uint64_t
ultrasonic_get_in (uint trig_pin, uint echo_pin)
{
    uint64_t pulse_length = ultrasonic_get_pulse(trig_pin, echo_pin);
    return (uint64_t)ULTRASONIC_PULSE_TO_IN(pulse_length);
}

// Private functions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Get the pulse length in cycles.
 *
 * @param trig_pin Trigger pin for the ultrasonic sensor.
 * @param echo_pin Echo pin for the ultrasonic sensor.
 * @return uint64_t The pulse length in cycles.
 */
static uint64_t
ultrasonic_get_pulse (uint trig_pin, uint echo_pin)
{
    gpio_put(trig_pin, 1);
    sleep_us(ULTRASONIC_TRIG_PULSE_US);
    gpio_put(trig_pin, 0);
    g_width = 0;
    return g_pulse_width;
}

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
        g_start_time = get_absolute_time();
        g_width++;
        if (ULTRASONIC_TIMEOUT < g_width)
        {
            g_width = 0;
        }
    }
    else if (GPIO_IRQ_EDGE_FALL == events)
    {
        g_end_time    = get_absolute_time();
        g_pulse_width = absolute_time_diff_us(g_start_time, g_end_time);
    }
}

// End of file driver/ultrasonic/ultrasonic.c.
