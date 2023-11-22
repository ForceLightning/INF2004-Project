/**
 * @file main.c
 *
 * @author Jurgen Tan
 *
 * @brief Demonstration for the magnetometer driver.
 *
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "magnetometer.h"

int
main ()
{
    stdio_init_all();
    stdio_usb_init();
    init_magnetometer();

    if (cyw43_arch_init())
    {
        printf("CYW4343X initialization failed.\n");
        return 1;
    }

    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        float true_heading = 0;
        float curr_heading = 0;
        readMagnetometerData(&true_heading, &curr_heading);
    }

    return 0;
}

// End of file driver/magnetometer/main.c.
