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
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

#define TRIG_PIN 0
#define ECHO_PIN 1

int
main (void)
{
    // Initialisation.
    // 
    stdio_init_all();
    init_ultrasonic_pins(TRIG_PIN, ECHO_PIN);
    
    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        uint64_t distance_cm = get_cm(TRIG_PIN, ECHO_PIN);
        uint64_t distance_inch = get_inches(TRIG_PIN, ECHO_PIN);
        printf("Distance in cm: %llu\n", distance_cm);
        printf("Distance in inches: %llu\n", distance_inch);

        sleep_ms(1000); 
    }

}
// End of driver/ultrasonic/main.c.
