#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdio.h"
#include "motor_control.h"
#include "pid.h"

turn_params_t g_turn_params;

/**
 * @brief Interrupt callback function on rising edge
 *
 * @param gpio      GPIO pin number.
 * @param events    Event mask. @see gpio_irq_level.
 */
void
encoder_tick_isr (uint gpio, uint32_t events)
{
    printf("Encoder step count: %d\n", g_turn_params.encoder_step_count);
    navigate_car_turn(&g_turn_params, g_turn_params.turn_direction);
}

int
main (void)
{
    // Initialisation.
    //
    stdio_init_all();
    init_pid_structs(&g_turn_params);

    // Initialize motors.
    motor_start(
        MOTOR_LEFT_PIN_CLKWISE, MOTOR_LEFT_PIN_ANTICLKWISE, MOTOR_PWM_PIN_LEFT);
    motor_start(MOTOR_RIGHT_PIN_CLKWISE,
                MOTOR_RIGHT_PIN_ANTICLKWISE,
                MOTOR_PWM_PIN_RIGHT);

    gpio_set_irq_enabled_with_callback(
        PID_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_tick_isr);

    for (;;)
    {
        // Get input from serial monitor
        char user_input = getchar();

        switch (user_input)
        {
            case '1':
            case '3':
            case '2':
                motor_move_forward();
                g_turn_params.b_is_turning   = 1;
                g_turn_params.turn_direction = user_input;
                break;
            default:
                break;
        }
    }
}
