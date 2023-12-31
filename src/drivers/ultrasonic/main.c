/**
 * @file main.c
 * @author Ang Jia Yu
 * @brief Demonstration for the Ultrasonic driver.
 * @version 0.1
 * @date 2023-10-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "pico/stdio.h"
#include "pico/time.h"
#include "ultrasonic/ultrasonic.h"

/**
 * @defgroup ultrasonic_demo_constants Ultrasonic Demo Constants
 * @brief Constants for the ultrasonic demo.
 * @{
 */

/** @brief Trigger pin for the ultrasonic sensor.*/
#define TRIG_PIN 0
/** @brief Echo pin for the ultrasonic sensor. */
#define ECHO_PIN 1
/** Number of milliseconds in a second. */
#define SLEEP_1S 1000
/**
 @} */ // End of ultrasonic_demo_constants group.

/**
 * @brief Main function for the ultrasonic demo.
 * 
 * @return int 0 if successful.
 */
int
main (void)
{
    // Initialisation.
    //
    stdio_init_all();
    ultrasonic_init_pins(TRIG_PIN, ECHO_PIN);

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        uint64_t distance_cm   = ultrasonic_get_cm(TRIG_PIN, ECHO_PIN);
        uint64_t distance_inch = ultrasonic_get_in(TRIG_PIN, ECHO_PIN);
        printf("Distance in cm: %llu\n", distance_cm);
        printf("Distance in inches: %llu\n", distance_inch);

        sleep_ms(SLEEP_1S);
    }
}

// End of file driver/ultrasonic/main.c.
