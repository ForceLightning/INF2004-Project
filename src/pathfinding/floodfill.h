/**
 * @file floodfill.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the function prototypes for the floodfill algorithm
 * for mapping of the maze.
 * @version 0.1
 * @date 2023-11-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <stdint.h>
#include "maze.h"

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 & @typedef floodfill_explore_func_t
 * @brief This function pointer type is used to explore the maze. It is expected
 * to return the walls that the robot sees.
 *
 * @param p_grid Pointer to the maze.
 * @param p_navigator Pointer to the navigator state.
 * @param direction Direction to explore.
 *
 * @return int16_t Returns a bitmask of the walls.
 */
typedef uint16_t (*floodfill_explore_func_t)(
    maze_grid_t              *p_grid,
    maze_navigator_state_t   *p_navigator,
    maze_cardinal_direction_t direction);

/**
 * @typedef floodfill_move_navigator_t
 * @brief Moves the navigator/robot in the specified direction.
 *
 * @param p_navigator Pointer to the navigator state.
 * @param direction Direction to move.
 */
typedef void (*floodfill_move_navigator_t)(maze_navigator_state_t *p_navigator,
                                           maze_cardinal_direction_t direction);

// Public Functions.
// ----------------------------------------------------------------------------
//

void floodfill_init_maze_nowall(maze_grid_t *p_grid);

void floodfill_map_maze(maze_grid_t               *p_grid,
                        const maze_grid_cell_t    *p_end_node,
                        maze_navigator_state_t    *p_navigator,
                        floodfill_explore_func_t   p_explore_func,
                        floodfill_move_navigator_t p_move_navigator);

#endif

// End of file pathfinding/floodfill.h
