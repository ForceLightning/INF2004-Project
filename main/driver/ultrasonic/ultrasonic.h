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

// Function prototypes
//
void     init_ultrasonic_pins(uint trig_pin, uint echo_pin);
uint64_t get_pulse(uint trig_pin, uint echo_pin);
uint64_t get_cm(uint trig_pin, uint echo_pin);
uint64_t get_inches(uint trig_pin, uint echo_pin);
#endif // ULTRASONIC_H

// End of file driver/ultrasonic/ultrasonic.h.
