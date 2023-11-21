/**
 * @file main.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief The main source file for robot control.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "irline/ir_sensor.h"
#include "magnetometer/magnetometer.h"
#include "motor/motor_control.h"
#include "ultrasonic/ultrasonic.h"
#include "encoder/wheel_encoder.h"
#include "wifi/wifi.h"

#include "pathfinding/a_star.h"

// Task priorities.
//
#ifndef MAIN_TASK_PRIORITY
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1ul)
#endif

// Unless otherwise specified, the default core to run FreeRTOS on is core 0.
//
#ifndef RUN_FREERTOS_ON_CORE
#define RUN_FREERTOS_ON_CORE 0
#endif

static void
main_task (__unused void *params)
{
    if (cyw43_arch_init())
    {
        printf("CYW4343X initialization failed!\n");
        return;
    }

    for (;;)
    {
        tight_loop_contents();
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
                MAIN_TASK_PRIORITY,
                &h_main_task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(h_main_task, 1);
#endif

    vTaskStartScheduler();
}

int
main (void)
{
    stdio_init_all();
    // init_magnetometer();
    tcp_server_begin_init();
    // readMagnetometerData();
    tcp_server_begin();
    

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
        tight_loop_contents(); // No-op
        // readMagnetometerData();
        // tcp_server_begin();
    }
#else
    printf("Starting %s on core 0:\n", rtos_name);
    v_launch();

#endif

    return 0;
}

// End of file comment.
