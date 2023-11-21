/**
 * @file pid.c
 * @author Bryan Seah
 * @brief
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */
// #include <stdbool.h>
// #include "hardware/pwm.h"
// #include "hardware/gpio.h"
// #include "pico/types.h"
#include "motor/motor_control.h"
#include "maze.h"
#include "pid.h"

/**
 * @brief Sets up pid struct(s).
 */
void
init_pid_structs (turn_params_t *p_turn_params)
{
    p_turn_params->is_turning         = 0;
    p_turn_params->encoder_step_count = 0;
    p_turn_params->turn_direction     = 'n';
    p_turn_params->is_centered        = 0;
    p_turn_params->completed_turn     = 0;
    p_turn_params->moved_cell         = 0;
}

/**
 * @brief Function to turn the car based on a given direction
 *
 * @param p_navigator
 * @param direction
 */
void
navigate_car_turn (turn_params_t            *p_turn_params,
                   maze_cardinal_direction_t direction)
{
    if (p_turn_params->is_turning)
    {
        p_turn_params->encoder_step_count++;

        // Center the car to prepare for turning
        if (!p_turn_params->is_centered)
        {
            if (p_turn_params->encoder_step_count == ENCODER_CENTER_OFFSET)
            {
                p_turn_params->is_centered        = 1;
                p_turn_params->encoder_step_count = 0;
            }
        }
        else if (!p_turn_params->completed_turn)
        {
            switch ((uint)direction)
            {
                case WEST:
                    turn_left(0);
                    if (p_turn_params->encoder_step_count
                        == ENCODER_STEP_TURN_90_DEG)
                    {
                        p_turn_params->completed_turn     = 1;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;

                case EAST:
                    turn_right(0);
                    if (p_turn_params->encoder_step_count
                        == ENCODER_STEP_TURN_90_DEG)
                    {
                        p_turn_params->completed_turn     = 1;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;

                case SOUTH:
                    turn_left(0);
                    if (p_turn_params->encoder_step_count
                        == ENCODER_STEP_TURN_180_DEG)
                    {
                        p_turn_params->completed_turn     = 1;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;
            }
        }
        else if (!p_turn_params->moved_cell)
        {
            move_forward();
            if (p_turn_params->encoder_step_count == ENCODER_STEP_MOVE)
            {
                p_turn_params->moved_cell         = 1;
                p_turn_params->encoder_step_count = 0;
            }
        }
        else
        {
            init_pid_structs(p_turn_params);
        }
    }
    else
    {
        return;
    }
}

// End of file pid.c
