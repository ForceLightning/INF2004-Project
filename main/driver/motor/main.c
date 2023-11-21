#include <stdio.h>
#include "motor_control.h"

// Definitions for the motor pins.
// 
#define LEFT_MOTOR_PIN_CLKWISE      16
#define LEFT_MOTOR_PIN_ANTICLKWISE  17
#define RIGHT_MOTOR_PIN_CLKWISE     15
#define RIGHT_MOTOR_PIN_ANTICLKWISE 14
#define PWM_PIN_LEFT                10
#define PWM_PIN_RIGHT               11

int
main (void)
{
    // Initialisation.
    //
    stdio_init_all();

    // motor_pins_t motor_data = {
    //     PWM_PIN_LEFT,
    //     PWM_PIN_RIGHT,
    //     LEFT_MOTOR_PIN_CLKWISE,
    //     LEFT_MOTOR_PIN_ANTICLKWISE,
    //     RIGHT_MOTOR_PIN_CLKWISE,
    //     RIGHT_MOTOR_PIN_ANTICLKWISE,
    // };

    // Initialize motors.
    start_motor(
        LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_LEFT);
    start_motor(
        RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_RIGHT);

    for (;;)
    {
        // Get input from serial monitor
        char user_input = getchar();

        switch (user_input)
        {
            case 'f':
                printf("Moving forward...\n");
                move_forward();
                break;
            case 's':
                printf("Stopping...\n");
                stop();
                break;
            case 'b':
                printf("Reversing...\n");
                reverse();
                break;
            case 'l':
                printf("Turning forward left...\n");
                turn_left(0);
                break;
            case 't':
                printf("Turning reverse left...\n");
                turn_left(1);
                break;
            case 'r':
                printf("Turning forward right...\n");
                turn_right(0);
                break;
            case 'y':
                printf("Turning forward right...\n");
                turn_right(1);
                break;
            default:
                printf("Invalid input. Stopping car...\n");
                stop();
                break;
        }
    }
}