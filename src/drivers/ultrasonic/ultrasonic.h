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

#ifndef ULTRASONIC_H // Include guard.
#define ULTRASONIC_H

// Definitions
// -----------------------------------------------------------------------------
//

/**
 * @defgroup ultrasonic_constants Ultrasonic constants
 * @brief Constants for the ultrasonic sensor.
 * @{
 */

/** @brief Timeout in cycles. */
#define ULTRASONIC_TIMEOUT 26100
/** @brief Trigger pulse width in us. */
#define ULTRASONIC_TRIG_PULSE_US 10
/**
 @} */ // End of ultrasonic_constants group.

/**
 * @defgroup ultrasonic_macros Ultrasonic macros
 * @brief Macros for the ultrasonic sensor.
 * @{
 */

/**
 * @def ULTRASONIC_PULSE_TO_CM(x)
 * @param[in] x Pulse width in us. Constants from datasheet.
 * @return Pulse width in cm.
 */
#define ULTRASONIC_PULSE_TO_CM(x) (x / 29. / 2.)

/**
 * @def ULTRASONIC_PULSE_TO_IN(x)
 * @param[in] x Pulse width in us. Constants from datasheet.
 * @return Pulse width in inches.
 */
#define ULTRASONIC_PULSE_TO_IN(x) (x / 74. / 2.)

/** @}*/ // End of ultrasonic_macros group.

// Function prototypes
// -----------------------------------------------------------------------------
//

void     ultrasonic_init_pins(uint trig_pin, uint echo_pin);
uint64_t ultrasonic_get_cm(uint trig_pin, uint echo_pin);
uint64_t ultrasonic_get_in(uint trig_pin, uint echo_pin);

#endif // ULTRASONIC_H

// End of file driver/ultrasonic/ultrasonic.h.
