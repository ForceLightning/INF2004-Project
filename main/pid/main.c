#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdio.h"
#include "motor/motor_control.h"
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
    start_motor(
        LEFT_MOTOR_PIN_CLKWISE, LEFT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_LEFT);
    start_motor(
        RIGHT_MOTOR_PIN_CLKWISE, RIGHT_MOTOR_PIN_ANTICLKWISE, PWM_PIN_RIGHT);

    gpio_set_irq_enabled_with_callback(
        ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_tick_isr);

    for (;;)
    {
        // Get input from serial monitor
        char user_input = getchar();

        switch (user_input)
        {
            case '1':
                move_forward();
                g_turn_params.is_turning     = 1;
                g_turn_params.turn_direction = user_input;
                break;

            case '3':
                move_forward();
                g_turn_params.is_turning     = 1;
                g_turn_params.turn_direction = user_input;
                break;

            case '2':
                move_forward();
                g_turn_params.is_turning     = 1;
                g_turn_params.turn_direction = user_input;
                break;
        }
    }
}
