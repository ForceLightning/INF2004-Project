/**
 * @file main.c
 * @author 
 * @brief Demonstration for the Ultrasonic driver.
 * @version 0.1
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ultrasonic.h"

int
main (void)
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("CYW4343X initialization failed.\n");
        return 1;
    }
    
    // TODO: Initialise the ultrasonic driver.
    
    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        tight_loop_contents(); // No-op
        // TODO: Read from ultrasonic driver.
    }

}

// End of driver/ultrasonic/main.c.
