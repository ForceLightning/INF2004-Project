/**
 * @file pid.c
 * @author Bryan Seah
 * @brief Driver for PID control.
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */
#include "motor/motor_control.h"
#include "pid/pid.h"
#include "magnetometer/magnetometer.h"
#include "pathfinding/maze.h"

/**
 * @brief Sets up pid struct(s).
 * @param[in,out] p_turn_params Pointer to the turn parameters.
 */
void
pid_init_structs (pid_turn_params_t *p_turn_params)
{
    p_turn_params->b_is_turning       = 0;
    p_turn_params->encoder_step_count = 0;
    p_turn_params->turn_direction     = 'n';
    p_turn_params->b_is_centered      = 0;
    p_turn_params->b_is_turn_complete = 0;
    p_turn_params->b_is_moved_cell    = 0;
}

/**
 * @brief Function to turn the car and move in a given direction
 *
 * @param[in,out] p_turn_params Pointer to the turn parameters.
 * @param[in] direction Direction to turn.
 */
void
pid_navigate_turn (pid_turn_params_t        *p_turn_params,
                   maze_cardinal_direction_t direction)
{
    if (p_turn_params->b_is_turning)
    {
        p_turn_params->encoder_step_count++;

        // Center the car to prepare for turning
        if (!p_turn_params->b_is_centered)
        {
            if (p_turn_params->encoder_step_count == PID_ENCODER_CENTER_OFFSET)
            {
                p_turn_params->b_is_centered      = 1;
                p_turn_params->encoder_step_count = 0;
            }
        }
        else if (!p_turn_params->b_is_turn_complete)
        {
            switch (direction)
            {
                case MAZE_WEST:
                    motor_turn_left(0);
                    if (p_turn_params->encoder_step_count
                        == PID_ENCODER_STEP_TURN_90_DEG)
                    {
                        p_turn_params->b_is_turn_complete = 1;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;

                case MAZE_EAST:
                    motor_turn_right(0);
                    if (p_turn_params->encoder_step_count
                        == PID_ENCODER_STEP_TURN_90_DEG)
                    {
                        p_turn_params->b_is_turn_complete = 1u;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;

                case MAZE_SOUTH:
                    motor_turn_left(0);
                    if (p_turn_params->encoder_step_count
                        == PID_ENCODER_STEP_TURN_180_DEG)
                    {
                        p_turn_params->b_is_turn_complete = 1u;
                        p_turn_params->encoder_step_count = 0;
                    }

                    break;

                default:
                    break;
            }
        }
        else if (!p_turn_params->b_is_moved_cell)
        {
            motor_move_forward();
            if (p_turn_params->encoder_step_count == PID_ENCODER_STEP_MOVE)
            {
                p_turn_params->b_is_moved_cell    = 1;
                p_turn_params->encoder_step_count = 0;
            }
        }
        else
        {
            pid_init_structs(p_turn_params);
        }
    }
    else
    {
        return;
    }
}

/**
 * @brief Sets defaults for PID parameters.
 *
 * @param[out] p_pid_params Pointer to the PID parameters.
 */
void
pid_init_error_correction (pid_params_t *p_pid_params)
{
    p_pid_params->k_p              = PID_KP;
    p_pid_params->k_i              = PID_KI;
    p_pid_params->k_d              = PID_KD;
    p_pid_params->epsilon          = PID_EPSILON;
    p_pid_params->ratio_to_bearing = PID_RATIO_TO_BEARING;
    p_pid_params->setpoint         = 0.0f;
    p_pid_params->integral         = 0.0f;
    p_pid_params->prev_error       = 0.0f;
    p_pid_params->current_ratio    = 0.0f;
    p_pid_params->current_bearing  = 0.0f;
}

/**
 * @brief Calculates the PID control signal.
 *
 * @param[in] current_bearing Current bearing of the car in degrees.
 * @param[in] target_bearing Target bearing of the car in degrees.
 * @param[in] current_ratio Current motor differential ratio.
 * @param[in,out] p_pid_params Pointer to the PID parameters.
 * @return float Control signal.
 */
float
pid_calculate_correction (float         current_bearing,
                          float         target_bearing,
                          float         current_ratio,
                          pid_params_t *p_pid_params)
{
    float error
        = PID_DEGREES_NORMALISE((int)(target_bearing - current_bearing));

    p_pid_params->integral += error;
    float derivative = error - p_pid_params->prev_error;

    float control_signal = p_pid_params->k_p * error
                           + p_pid_params->k_i * p_pid_params->integral
                           + p_pid_params->k_d * derivative;

    p_pid_params->prev_error = error;

    return control_signal;
}

/**
 * @brief Updates the motor differential ratio based on the target bearing and
 * current bearing.
 *
 * @param[in] target_bearing Target bearing of the car in degrees.
 * @param[out] current_bearing Current bearing of the car in degrees.
 * @param[in,out] p_pid_params Pointer to the PID parameters.
 */
void
pid_bearing_correction (float         target_bearing,
                        float         current_bearing,
                        pid_params_t *p_pid_params)
{
    // pid_init_error_correction(pid_params_t *p_pid_params);

    p_pid_params->current_bearing = current_bearing;
    while ((int)p_pid_params->current_bearing != (int)target_bearing)
    {
        float control_signal
            = pid_calculate_correction(p_pid_params->current_bearing,
                                       target_bearing,
                                       p_pid_params->current_ratio,
                                       p_pid_params);

        float new_ratio = (p_pid_params->current_ratio + p_pid_params->epsilon)
                          + ((control_signal + p_pid_params->epsilon + 1)
                             * p_pid_params->ratio_to_bearing);

        // p_pid_params->current_bearing += (new_ratio -
        // p_pid_params->current_ratio) / p_pid_params->ratio_to_bearing;
        p_pid_params->current_bearing = magneto_get_curr_bearing();
        p_pid_params->current_ratio   = new_ratio;

        motor_update_ratio(p_pid_params->current_ratio);
    }

    motor_update_ratio(PID_BIAS_LEFT_RATIO);
}

// End of file pid.c
