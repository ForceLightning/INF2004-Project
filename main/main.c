// /**
//  * @file main.c
//  * @author Christopher Kok (chris@forcelightning.xyz)
//  * @brief The main source file for robot control.
//  * @version 0.1
//  * @date 2023-11-07
//  *
//  * @copyright Copyright (c) 2023
//  *
//  */

// #include <stdio.h>
// #include "pico/cyw43_arch.h"
// #include "pico/stdlib.h"

// #include "FreeRTOS.h"
// #include "task.h"
// #include "message_buffer.h"

// // #include "irline/ir_sensor.h"
// #include "magnetometer/magnetometer.h"
// #include "motor/motor_control.h"
// #include "ultrasonic/ultrasonic.h"
// #include "encoder/wheel_encoder.h"
// #include "pid/pid.h"
// #include "irline/ir_sensor.h"
// // #include "wifi/wifi.h"

// #include "pathfinding/a_star.h"

// volatile uint obstacle_detected = 0;

// // Task priorities.
// //
// #ifndef MAIN_TASK_PRIORITY
// #define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1ul)
// #endif

// // Unless otherwise specified, the default core to run FreeRTOS on is core 0.
// //
// #ifndef RUN_FREERTOS_ON_CORE
// #define RUN_FREERTOS_ON_CORE 0
// #endif
// // volatile bearing_data_t *gp_bearing_data;

// static void
// main_task (__unused void *params)
// {
//     if (cyw43_arch_init())
//     {
//         printf("CYW4343X initialization failed!\n");
//         return;
//     }

//     for (;;)
//     {
//         tight_loop_contents();
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }

//     cyw43_arch_deinit();
// }


// static void 
// read_magnetometer_task(__unused void *params)
// {
//     init_bearing_data();
//     readMagnetometerData();
// }

// static void
// move_car_forward_task(__unused void *params)
// {
//     start_motor(
//         LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_LEFT);
//     start_motor(
//         RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_RIGHT);
//     while(1){
//             pid_params_t pid_params;
//             init_pid_error_correction(&pid_params);
//             bearing_correction(&pid_params);
//         move_forward();
//     }
// }

// static void
// ultrasonic_detect_obstacle(__unused void *params)
// {
//     init_ultrasonic_pins(TRIG_PIN, ECHO_PIN);
//     // get_cm(TRIG_PIN, ECHO_PIN);
//     get_pulse(TRIG_PIN, ECHO_PIN);
    
    
// }

// static void
// ir_wall_detection(__unused void *params)
// {
//     setup_gpio_pins(GPIO_PIN_LEFT, GPIO_PIN_FRONT);
// }

// static void
// v_launch (void)
// {
//     // TaskHandle_t magnetometer_task;
//     // xTaskCreate(read_magnetometer_task,
//     //             "Magnetometer",
//     //             configMINIMAL_STACK_SIZE,
//     //             NULL,
//     //             MAIN_TASK_PRIORITY,
//     //             &magnetometer_task);

//     // TaskHandle_t move_car_forward_handle;
//     // xTaskCreate(move_car_forward_task,
//     //             "Move Car Forward",
//     //             configMINIMAL_STACK_SIZE,
//     //             NULL,
//     //             MAIN_TASK_PRIORITY,
//     //             &move_car_forward_handle);

//     TaskHandle_t ultrasonic_handle;
//     xTaskCreate(ultrasonic_detect_obstacle,
//                 "Ultrasonic",
//                 configMINIMAL_STACK_SIZE,
//                 NULL,
//                 MAIN_TASK_PRIORITY,
//                 &ultrasonic_handle);

//     // TaskHandle_t ir_handle;
//     // xTaskCreate(ir_wall_detection,
//     //             "IR",
//     //             configMINIMAL_STACK_SIZE,
//     //             NULL,
//     //             MAIN_TASK_PRIORITY,
//     //             &ir_handle);

// #if NO_SYS && configUSE_CORE_AFFINITY && configNUM_CORES > 1
//     vTaskCoreAffinitySet(h_main_task, 1);
// #endif

// vTaskStartScheduler();

// }

// int
// main (void)
// {
//     stdio_init_all();
//     scanf("Press any key to continue...\n");
//     init_magnetometer();
//     //tcp_server_begin_init();
//     //tcp_server_begin();
    

//     const char *rtos_name;

// #if (portSUPPORT_SMP == 1)
//     rtos_name = "FreeRTOS SMP";
// #else
//     rtos_name = "FreeRTOS";
// #endif

// #if (portSUPPORT_SMP == 1) && (configNUM_CORES == 2)
//     printf("Running %s on %d cores\n", rtos_name, configNUM_CORES);
//     v_launch();
// #elif (RUN_FREERTOS_ON_CORE == 1)
//     printf("Running %s on core 1\n", rtos_name);
//     multicore_launch_core1(v_launch);

//     for (;;)
//     {
//         tight_loop_contents(); // No-op
//         // readMagnetometerData();
//         // tcp_server_begin();
//     }
// #else
//     printf("Starting %s on core 0:\n", rtos_name);
//     v_launch();

// #endif

//     return 0;
// }

// // End of file comment.

#include <stdio.h>
#include "pico/stdlib.h"

#include "ultrasonic/ultrasonic.h"
#include "motor/motor_control.h"
#include "pid/pid.h"
#include "magnetometer/magnetometer.h"
#include "irline/ir_sensor.h"

uint obstacle_detected = 0;
uint avoid_obstacle_fired = 0;
uint left_corner = 0;
uint readjust = 0;
turn_params_t g_turn_params;

// void
// encoder_tick_isr (uint gpio, uint32_t events)
// {
//     if(avoid_obstacle_fired){
//         navigate_car_turn(&g_turn_params, g_turn_params.turn_direction);
//     }
// }

int main(){
    stdio_init_all();
    stdio_usb_init();

    sleep_ms(5000);

    init_ultrasonic_pins(TRIG_PIN, ECHO_PIN);
    start_motor(
        LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_LEFT);
    start_motor(
        RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_RIGHT);

    setup_gpio_pins(GPIO_PIN_LEFT, GPIO_PIN_FRONT);

    // init_magnetometer();
    // bearing_data_t bearing_data;
    
    // init_bearing_data(&bearing_data);


    for(;;){
        // readMagnetometerData(&bearing_data);
        // printf("True north: %f\n", getTrueBearing(&bearing_data));
        // printf("Current Bearing: %f\n", getCurrentBearing(&bearing_data));
        
        
        if(obstacle_detected || left_corner){
            stop();
        } else {
            move_forward();
        }

        if(!readjust){
            update_ratio(1.05f);
            move_forward();
        }

        // sleep_ms(2000);

        // g_turn_params.is_turning     = 1;
        // g_turn_params.turn_direction = 'r';

        // while(!g_turn_params.completed_turn){
        //     navigate_car_turn(&g_turn_params, g_turn_params.turn_direction);
        //     sleep_ms(24);
        // }

        if(left_corner){
            g_turn_params.is_turning     = 1;
            g_turn_params.turn_direction = 'r';

            while(!g_turn_params.completed_turn){
                navigate_car_turn(&g_turn_params, g_turn_params.turn_direction);
                sleep_ms(24);
            }
            init_pid_structs(&g_turn_params);

            left_corner = 0;
        
        }

        if(readjust){
            update_ratio(0.95f);
            move_forward();
            sleep_ms(1000);
            readjust = 0;
        }
        
        if(avoid_obstacle_fired){
            move_forward();
            g_turn_params.is_turning     = 1;
            g_turn_params.turn_direction = 'b';

            while(!g_turn_params.completed_turn){
                navigate_car_turn(&g_turn_params, g_turn_params.turn_direction);
                sleep_ms(24);
            }
            init_pid_structs(&g_turn_params);

            avoid_obstacle_fired = 0;
            obstacle_detected = 0;
        }
        
        uint64_t distance_cm   = get_cm(TRIG_PIN, ECHO_PIN);
        printf("Distance in cm: %llu\n", distance_cm);

        if(distance_cm <= 25 && distance_cm != 0){
            obstacle_detected = 1;
            avoid_obstacle_fired = 1;
        }

        uint16_t wall_mask = find_wall_directions(GPIO_PIN_LEFT, GPIO_PIN_FRONT);
        printf("Wall mask: %d\n", wall_mask);
        if(wall_mask == 9){
            left_corner = 1;
        } else if(wall_mask == 8){
            readjust = 1;
        }

        // sleep_ms(SLEEP_1S);

    }
}