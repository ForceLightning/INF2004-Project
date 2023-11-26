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
#include "motor_control.h"
#include "maze.h"
#include "pid.h"
#include "magnetometer.h"
#include <stdio.h>

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

void init_pid_error_correction(pid_params_t *p_pid_params){
    p_pid_params->kp = 0.01f;
    p_pid_params->ki = 0.05f;
    p_pid_params->kd = 0.01f;
    p_pid_params->epsilon = 0.01f;
    p_pid_params->ratio_to_bearing = -0.035f;
    p_pid_params->setpoint = 0.0f;
    p_pid_params->integral = 0.0f;
    p_pid_params->prev_error = 0.0f;
    p_pid_params->current_ratio = 0.0f;
    p_pid_params->current_bearing = 0.0f;
}

float calculate_pid(float current_bearing, float target_bearing, float current_ratio, pid_params_t *p_pid_params){
    float error = (float)((int)(target_bearing - current_bearing + 180) % 360 - 180);

    p_pid_params->integral += error;
    float derivative = error - p_pid_params->prev_error;

    float control_signal = p_pid_params->kp * error + p_pid_params->ki * p_pid_params->integral + p_pid_params->kd * derivative;

    p_pid_params->prev_error = error;

    return control_signal;
}

void bearing_correction(float target_bearing, float current_bearing, pid_params_t *p_pid_params){ 
    // init_pid_error_correction(pid_params_t *p_pid_params);
    
    p_pid_params->current_bearing = current_bearing;
    while((int)p_pid_params->current_bearing != (int)target_bearing){
        float control_signal = calculate_pid(p_pid_params->current_bearing, target_bearing, p_pid_params->current_ratio, p_pid_params);

        float new_ratio = (p_pid_params->current_ratio + p_pid_params->epsilon) + ((control_signal + p_pid_params->epsilon + 1) * p_pid_params->ratio_to_bearing);

        // p_pid_params->current_bearing += (new_ratio - p_pid_params->current_ratio) / p_pid_params->ratio_to_bearing;
        p_pid_params->current_bearing = magneto_get_curr_bearing();
        p_pid_params->current_ratio = new_ratio;

        update_ratio(p_pid_params->current_ratio);

    }   

    update_ratio(1.05f);
}

// End of file pid.c
