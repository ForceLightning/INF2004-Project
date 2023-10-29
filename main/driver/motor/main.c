#include <stdio.h>
#include "motor_control.h"

// Definitions for the motor pins.
// 
#define LEFT_MOTOR_PIN_CLKWISE      16
#define LEFT_MOTOR_PIN_ANTICLKWISE  17
#define RIGHT_MOTOR_PIN_CLKWISE     14
#define RIGHT_MOTOR_PIN_ANTICLKWISE 15
#define PWM_PIN_LEFT                0
#define PWM_PIN_RIGHT               1

int
main (void)
{
    // Initialisation.
    //
    stdio_init_all();

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
                move_forward(PWM_PIN_LEFT,
                             PWM_PIN_RIGHT,
                             LEFT_MOTOR_PIN_CLKWISE,
                             LEFT_MOTOR_PIN_ANTICLKWISE,
                             RIGHT_MOTOR_PIN_CLKWISE,
                             RIGHT_MOTOR_PIN_ANTICLKWISE);
                break;
            case 's':
                printf("Stopping...\n");
                stop(PWM_PIN_LEFT, PWM_PIN_RIGHT);
                break;
            case 'b':
                printf("Reversing...\n");
                reverse(PWM_PIN_LEFT,
                        PWM_PIN_RIGHT,
                        LEFT_MOTOR_PIN_CLKWISE,
                        LEFT_MOTOR_PIN_ANTICLKWISE,
                        RIGHT_MOTOR_PIN_CLKWISE,
                        RIGHT_MOTOR_PIN_ANTICLKWISE);
                break;
            case 'l':
                printf("Turning forward left...\n");
                turn_left(PWM_PIN_LEFT,
                          PWM_PIN_RIGHT,
                          LEFT_MOTOR_PIN_CLKWISE,
                          LEFT_MOTOR_PIN_ANTICLKWISE,
                          RIGHT_MOTOR_PIN_CLKWISE,
                          RIGHT_MOTOR_PIN_ANTICLKWISE,
                          0);
                break;
            case 't':
                printf("Turning reverse left...\n");
                turn_left(PWM_PIN_LEFT,
                          PWM_PIN_RIGHT,
                          LEFT_MOTOR_PIN_CLKWISE,
                          LEFT_MOTOR_PIN_ANTICLKWISE,
                          RIGHT_MOTOR_PIN_CLKWISE,
                          RIGHT_MOTOR_PIN_ANTICLKWISE,
                          1);
                break;
            case 'r':
                printf("Turning forward right...\n");
                turn_right(PWM_PIN_LEFT,
                           PWM_PIN_RIGHT,
                           LEFT_MOTOR_PIN_CLKWISE,
                           LEFT_MOTOR_PIN_ANTICLKWISE,
                           RIGHT_MOTOR_PIN_CLKWISE,
                           RIGHT_MOTOR_PIN_ANTICLKWISE,
                           0);
                break;
            case 'y':
                printf("Turning forward right...\n");
                turn_right(PWM_PIN_LEFT,
                           PWM_PIN_RIGHT,
                           LEFT_MOTOR_PIN_CLKWISE,
                           LEFT_MOTOR_PIN_ANTICLKWISE,
                           RIGHT_MOTOR_PIN_CLKWISE,
                           RIGHT_MOTOR_PIN_ANTICLKWISE,
                           1);
                break;
            default:
                printf("Invalid input. Stopping car...\n");
                stop(PWM_PIN_LEFT, PWM_PIN_RIGHT);
                break;
        }
    }
}