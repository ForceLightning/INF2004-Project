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

#include "../../pathfinding/maze.h"

// Definitions for the encoder steps.
// 
#define ENCODER_STEP_TURN_90_DEG 18
#define ENCODER_STEP_TURN_180_DEG 36
#define ENCODER_STEP_MOVE 25
#define ENCODER_CENTER_OFFSET 5

#define ENCODER_PIN 21

struct turn_params_t {
    uint is_turning;
    uint encoder_step_count;
    char turn_direction;
    uint is_centered;
    uint completed_turn;
    uint moved_cell;
} turn_params;

/**
 * @brief Sets up pid struct(s).
 */
void init_pid_structs (void);

/**
 * @brief Function to turn the car based on a given direction
 * 
 * @param p_navigator 
 * @param direction 
 */
void navigate_car_turn(maze_cardinal_direction_t direction);

#endif // PID_H

// End of file pid.h
