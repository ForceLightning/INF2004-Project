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

#include "pathfinding/maze.h"

/**
 * @defgroup pid_encoder_constants PID Encoder Constants
 * @brief Constants for the encoder steps.
 * @{
 */

/** @brief Number of encoder steps to turn 90 deg.*/
#define PID_ENCODER_STEP_TURN_90_DEG 18
/** @brief Number of encoder steps to turn 180 deg.*/
#define PID_ENCODER_STEP_TURN_180_DEG 36
/** @brief Number of encoder steps to move 1 cell.*/
#define PID_ENCODER_STEP_MOVE 25
/** @brief Number of encoder steps from the center of a cell*/
#define PID_ENCODER_CENTER_OFFSET 5
/**
 * @} */ // End of pid_encoder_constants group.

/**
 * @defgroup pid_constants PID Constants
 * @brief Constants for the PID controller.
 * @{
 */

/** @brief Gain for proportional term.*/
#define PID_KP 0.01f
/** @brief Gain for integral term.*/
#define PID_KI 0.05f
/** @brief Gain for derivative term.*/
#define PID_KD 0.01f
/** @brief Epsilon for PID. A small value to prevent division by 0.*/
#define PID_EPSILON 0.01f
/** @brief Ratio to bearing for PID.*/
#define PID_RATIO_TO_BEARING -0.035f
/** @brief Encoder pin for PID.*/
#define PID_ENCODER_PIN 21
/** @brief Bias for left ratio.*/
#define PID_BIAS_LEFT_RATIO 1.05f

/**
 @} */ // End of pid_constants group.

/**
 * @def PID_DEGREES_NORMALISE(x)
 * @brief Normalises degrees to be between 0 and 360.
 * @param[in] x Degrees to normalise.
 * @return Normalised degrees.
 */
#define PID_DEGREES_NORMALISE(x) ((x + 180) % 360 - 180) // Normalise degrees.

// Type definitions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Struct for PID parameters. @see pid_params
 *
 */
typedef struct pid_params
{
    float k_p;              ///< Gain for proportional term.
    float k_i;              ///< Gain for integral term.
    float k_d;              ///< Gain for derivative term.
    float epsilon;          ///< Small epsilon value to prevent division by 0.
    float ratio_to_bearing; ///< Ratio to bearing for PID.
    float setpoint;         ///< Setpoint for PID.
    float integral;         ///< Integral for PID.
    float prev_error;       ///< Previous error for PID.
    float current_ratio;    ///< Current ratio for PID.
    float current_bearing;  ///< Current bearing for PID.
} pid_params_t;

/**
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

// Public function declarations.
// -----------------------------------------------------------------------------
//

void  pid_init_error_correction(pid_params_t *p_pid_params);
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
