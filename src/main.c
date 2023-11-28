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
#include <stddef.h>
#include <sys/cdefs.h>
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
#include "pid/pid.h"
#include "wifi/wifi.h"

#include "pathfinding/a_star.h"

// Definitions.
// ----------------------------------------------------------------------------
//

/**
 * @defgroup main_constants Main Constants
 * @brief Constants for the main program.
 * @{
 */

// Task priorities.
//
#ifndef MAIN_TASK_PRIORITY
/** @brief Priority of the main task. */
#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1ul)
#endif

// Unless otherwise specified, the default core to run FreeRTOS on is core 0.
//
#ifndef RUN_FREERTOS_ON_CORE
/** @brief Core to run FreeRTOS on. */
#define RUN_FREERTOS_ON_CORE 0
#endif

// Private function prototypes.
// ----------------------------------------------------------------------------
//

static void read_magnetometer_task(__unused void *p_params);
static void move_car_forward_task(__unused void *p_params);
static void tcp_server_begin_task(__unused void *p_params);
static void v_launch(void);

/**
 * @brief Main function for the robot. This just starts the tasks.
 *
 * @return int 0 if successful.
 */
int
main (void)
{
    stdio_init_all();
    // stdio_usb_init();
    magneto_init();
    wifi_tcp_server_begin_init();
    // gpio_init(20);

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
    }
#else
    // if (gpio_get(20)) {
    printf("Running %s on core 1\n", rtos_name);
    v_launch();
    // }

#endif

    return 0;
}

// Private functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Task to read the magnetometer.
 *
 * @param params Unused.
 */
static void
read_magnetometer_task (__unused void *p_params)
{
    magneto_read_data();
}

/**
 * @brief Task to move the car forward.
 *
 * @param params Unused.
 */
static void
move_car_forward_task (__unused void *p_params)
{
    motor_start(
        MOTOR_LEFT_PIN_CLKWISE, MOTOR_LEFT_PIN_ANTICLKWISE, MOTOR_PWM_PIN_LEFT);
    motor_start(MOTOR_RIGHT_PIN_CLKWISE,
                MOTOR_RIGHT_PIN_ANTICLKWISE,
                MOTOR_PWM_PIN_RIGHT);
    for (;;)
    {
        if (magneto_is_bearing_invalid())
        {
            pid_params_t pid_params;
            pid_init_error_correction(&pid_params);
            pid_bearing_correction(magneto_get_true_bearing(),
                                   magneto_get_curr_bearing(),
                                   &pid_params);
        }
        motor_move_forward();
    }
}

/**
 * @brief Task to begin the TCP server.
 *
 * @param params Unused.
 */
static void
tcp_server_begin_task (__unused void *p_params)
{
    wifi_tcp_server_begin();
}

/**
 * @brief Launches all tasks.
 *
 */
static void
v_launch (void)
{
    TaskHandle_t magnetometer_task;
    xTaskCreate(read_magnetometer_task,
                "Magnetometer",
                configMINIMAL_STACK_SIZE,
                NULL,
                MAIN_TASK_PRIORITY,
                &magnetometer_task);

    // TaskHandle_t move_car_forward_handle;
    // xTaskCreate(move_car_forward_task,
    //             "Move Car Forward",
    //             configMINIMAL_STACK_SIZE,
    //             NULL,
    //             MAIN_TASK_PRIORITY,
    //             &move_car_forward_handle);

    TaskHandle_t wifi_task;
    xTaskCreate(tcp_server_begin_task,
                "Wifi",
                configMINIMAL_STACK_SIZE,
                NULL,
                MAIN_TASK_PRIORITY,
                &wifi_task);

#if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
    vTaskCoreAffinitySet(h_main_task, 1);
#endif

    vTaskStartScheduler();
}

// End of file comment.
