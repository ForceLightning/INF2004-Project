/**
 * @file main.c
 * 
 * @author Jurgen Tan
 * 
 * @brief Demonstration for the WiFi driver.
 * 
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#include "wifi.h"
#include "pico/cyw43_arch.h"

int main()
{
    tcp_server_begin_init();

    // TODO: Setup GPIO.
    for (;;) // Loop forever. See Barr Group "Embedded C Coding Standard" 8.4.c
    {
        tcp_server_begin();
    }
    return 0;
}