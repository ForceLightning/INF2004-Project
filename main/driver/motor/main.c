/**
 * @file main.c
 * @author 
 * @brief Demonstration for the Motor controller driver.
 * @version 0.1
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/pwm.h"
#include "motor_control.h"

int
main (void)
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("CYW4343X initialization failed.\n");
        return 1;
    }
    
    // TODO: Drive forward/back.
    // TODO: Turn left/right.
    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        tight_loop_contents(); // No-op
    }

}

// End of driver/motor/main.c.
