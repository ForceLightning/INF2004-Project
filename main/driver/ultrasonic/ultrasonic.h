/**
 * @file ultrasonic.h
 * @author Ang Jia Yu
 * @brief Header file for the ultrasonic sensor driver.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef ULTRASONIC_H
#define ULTRASONIC_H
// Definitions
//
#define ULTRASONIC_TIMEOUT       26100 // Timeout in cycles.
#define ULTRASONIC_TRIG_PULSE_US 10    // Trigger pulse width in us.

#define TRIG_PIN 2    // Trigger pin for the ultrasonic sensor.
#define ECHO_PIN 3    // Echo pin for the ultrasonic sensor.
#define SLEEP_1S 1000 // 1 second in milliseconds.

typedef struct ultrasonic_data {
    absolute_time_t g_start_time;  // Start time of the pulse.
    absolute_time_t g_end_time;    // End time of the pulse.
    uint64_t        g_pulse_width; // Pulse width in us.
    uint64_t        g_width;       // Pulse width in cycles.
} ultrasonic_data_t;

// Function prototypes
//
void     init_ultrasonic_pins(uint trig_pin, uint echo_pin);
uint64_t get_pulse(uint trig_pin, uint echo_pin);
uint64_t get_cm(uint trig_pin, uint echo_pin);
uint64_t get_inches(uint trig_pin, uint echo_pin);
#endif // ULTRASONIC_H

// End of file driver/ultrasonic/ultrasonic.h.
