/**
 * @file pid.h
 * @author Bryan Seah
 * @brief
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#ifndef PID_H // Include guard.
#define PID_H

#include "maze.h"

// Definitions for the encoder steps.
//
#define PID_ENCODER_STEP_TURN_90_DEG \
    18 // Number of encoder steps to turn 90 deg.
#define PID_ENCODER_STEP_TURN_180_DEG \
    36                               // Number of encoder steps to turn 180 deg.
#define PID_ENCODER_STEP_MOVE     25 // Number of encoder steps to move 1 cell.
#define PID_ENCODER_CENTER_OFFSET 5  // Number of encoder steps to center.

// Constants for the PID.
//
#define PID_KP               0.01f   // Gain for proportional term.
#define PID_KI               0.05f   // Gain for integral term.
#define PID_KD               0.01f   // Gain for derivative term.
#define PID_EPSILON          0.01f   // Epsilon for PID.
#define PID_RATIO_TO_BEARING -0.035f // Ratio to bearing for PID.
#define PID_ENCODER_PIN      21      // Encoder pin for PID.
#define PID_BIAS_LEFT_RATIO  1.05f   // Bias for left ratio.

#define PID_DEGREES_NORMALISE(x) ((x + 180) % 360 - 180) // Normalise degrees.

/**
 * @typedef pid_params_t
 * @brief Struct for PID parameters.
 *
 */
typedef struct pid_params
{
    float k_p; ///< Gain for proportional term.
    float k_i; ///< Gain for integral term.
    float k_d; ///< Gain for derivative term.

    float epsilon;          ///< Small epsilon value to prevent division by 0.
    float ratio_to_bearing; ///< Ratio to bearing for PID.
    float setpoint;         ///< Setpoint for PID.
    float integral;         ///< Integral for PID.
    float prev_error;       ///< Previous error for PID.
    float current_ratio;    ///< Current ratio for PID.
    float current_bearing;  ///< Current bearing for PID.
} pid_params_t;

void init_pid_error_correction(pid_params_t *p_pid_params);

/**
 * @typedef turn_params_t
 * @brief Struct for turn parameters.
 */
typedef struct pid_turn_params
{
    uint b_is_turning;       ///< Flag to indicate if the car is turning.
    uint encoder_step_count; ///< Current encoder step count.
    char turn_direction;     ///< Direction to turn.
    uint b_is_centered;      ///< Flag to indicate if the car is centered.
    uint b_is_turn_complete; ///< Flag to indicate if the turn is complete.
    uint b_is_moved_cell;    ///< Flag to indicate if the car has moved a cell.
} pid_turn_params_t;

void  pid_init_structs(pid_turn_params_t *p_turn_params);
void  pid_navigate_turn(pid_turn_params_t        *p_turn_params,
                        maze_cardinal_direction_t direction);
float pid_calculate_correction(float         current_bearing,
                               float         target_bearing,
                               float         current_ratio,
                               pid_params_t *p_pid_params);
void  pid_bearing_correction(float         target_bearing,
                             float         current_bearing,
                             pid_params_t *p_pid_params);

#endif // PID_H

// End of file pid.h
