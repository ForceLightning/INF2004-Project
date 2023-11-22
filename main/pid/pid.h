/**
 * @file pid.h
 * @author Bryan Seah
 * @brief
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#ifndef PID_H
#define PID_H

#include "maze.h"
#include "magnetometer/magnetometer.h"

// Definitions for the encoder steps.
//
#define ENCODER_STEP_TURN_90_DEG  18
#define ENCODER_STEP_TURN_180_DEG 36
#define ENCODER_STEP_MOVE         25
#define ENCODER_CENTER_OFFSET     5

#define ENCODER_PIN 21

typedef struct pid_params {
    float kp;
    float ki;
    float kd;

    float epsilon;
    float ratio_to_bearing;
    float setpoint;
    float integral;
    float prev_error;
    float current_ratio;
    float current_bearing;
} pid_params_t;

void init_pid_error_correction(pid_params_t *p_pid_params);

typedef struct turn_params
{
    uint is_turning;
    uint encoder_step_count;
    char turn_direction;
    uint is_centered;
    uint completed_turn;
    uint moved_cell;
} turn_params_t;

/**
 * @brief Sets up pid struct(s).
 */
void init_pid_structs(turn_params_t *p_turn_params);

/**
 * @brief Function to turn the car based on a given direction
 *
 * @param p_navigator
 * @param direction
 */
void navigate_car_turn(turn_params_t            *p_turn_params,
                       maze_cardinal_direction_t direction);

float calculate_pid(float current_bearing, float target_bearing, float current_ratio, pid_params_t *p_pid_params);
void bearing_correction(pid_params_t *p_pid_params);

#endif // PID_H

// End of file pid.h
