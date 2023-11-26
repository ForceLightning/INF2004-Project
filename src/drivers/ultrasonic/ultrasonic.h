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
#define ULTRASONIC_PULSE_TO_CM(x) \
    (x / 29. / 2.) // Convert pulse width to cm. Constants from datasheet.
#define ULTRASONIC_PULSE_TO_IN(x) \
    (x / 74. / 2.) // Convert pulse width to in. Constants from datasheet.

// Function prototypes
//
void     ultrasonic_init_pins(uint trig_pin, uint echo_pin);
uint64_t ultrasonic_get_cm(uint trig_pin, uint echo_pin);
uint64_t ultrasonic_get_in(uint trig_pin, uint echo_pin);
#endif // ULTRASONIC_H

// End of file driver/ultrasonic/ultrasonic.h.
