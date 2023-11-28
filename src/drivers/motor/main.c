/**
 * @file main.c
 * @author Bryan Seah
 * @brief Demonstration for the motor driver.
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "motor/motor_control.h"

int
main (void)
{
    // Initialisation.
    //
    stdio_init_all();

    // Initialize motors.
    motor_start(
        MOTOR_LEFT_PIN_CLKWISE, MOTOR_LEFT_PIN_ANTICLKWISE, MOTOR_PWM_PIN_LEFT);
    motor_start(
        MOTOR_RIGHT_PIN_CLKWISE, MOTOR_RIGHT_PIN_ANTICLKWISE, MOTOR_PWM_PIN_RIGHT);

    for (;;)
    {
        // Get input from serial monitor
        char user_input = getchar();

        switch (user_input)
        {
            case 'f':
                printf("Moving forward...\n");
                motor_move_forward();
                break;
            case 's':
                printf("Stopping...\n");
                motor_stop();
                break;
            case 'b':
                printf("Reversing...\n");
                motor_reverse();
                break;
            case 'l':
                printf("Turning forward left...\n");
                motor_turn_left(0);
                break;
            case 't':
                printf("Turning reverse left...\n");
                motor_turn_left(1);
                break;
            case 'r':
                printf("Turning forward right...\n");
                motor_turn_right(0);
                break;
            case 'y':
                printf("Turning forward right...\n");
                motor_turn_right(1);
                break;
            default:
                printf("Invalid input. Stopping car...\n");
                motor_stop();
                break;
        }
    }
}

// End of file drivers/motor/main.c
