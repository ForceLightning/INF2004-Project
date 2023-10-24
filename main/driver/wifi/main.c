/**
 * @file main.c
 * @author
 * @brief Demonstration for the WiFi driver.
 * @version 0.1
 * @date 2023-10-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/lwiperf.h"

#include "FreeRTOS.h"
#include "task.h"

#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

#define TEST_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)

#if CLIENT_TEST && !defined(IPERF_SERVER_IP)
#error IPERF_SERVER_IP not defined
#endif

void
main_task (__unused void *params)
{

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return;
    }

    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(
            WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000))
    {
        printf("failed to connect.\n");
        exit(1);
    }
    else
    {
        printf("Connected.\n");
    }

    cyw43_arch_lwip_begin();

    // TODO: Add tcp client initialisation here.

    cyw43_arch_lwip_end();

    for (;
         ;) // Loop forever, see Barr Group's "Embedded C Coding Standard" 6.3.1
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    cyw43_arch_deinit();
}

static void
v_launch (void)
{
    TaskHandle_t h_main_task;
    xTaskCreate(main_task,
                "MainThread",
                configMINIMAL_STACK_SIZE,
                NULL,
                TEST_TASK_PRIORITY,
                &h_main_task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(h_main_task, 1);
#endif

    // Start the scheduler.
    vTaskStartScheduler();
}

int
main (void)
{
    stdio_init_all();

    const char *rtos_name;

#if (portSUPPORT_SMP == 1)
    rtos_name = "FreeRTOS SMP";
#else
    rtos_name = "FreeRTOS";
#endif

#if (portSUPPORT_SMP == 1) && (configNUM_CORES == 2)
    printf("Running %s on %d cores\n", rtos_name, configNUM_CORES);
    v_launch();
#elif (RUN_FREERTOS_ON_CORE == 1)
    printf("Running %s on core 1\n", rtos_name);
    multicore_launch_core1(v_launch);

    for (;;)
    {
        tight_loop_contents();
    }
#else
    printf("Starting %s on core 0:\n", rtos_name);
    v_launch();
#endif

    return 0;
}

// End of driver/wifi/main.c.
