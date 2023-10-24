/**
 * @file main.c
 * @author 
 * @brief Demonstration for the IR line sensor.
 * @version 0.1
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "ir_sensor.h"

int
main (void)
{
    stdio_init_all();
    adc_init();

    if (cyw43_arch_init())
    {
        printf("CYW4343X initialization failed.\n");
        return 1;
    }

    // TODO: Setup GPIO.

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        // TODO: Read from ADC.
        tight_loop_contents(); // No-op
    }

}

// End of driver/irline/main.c.
