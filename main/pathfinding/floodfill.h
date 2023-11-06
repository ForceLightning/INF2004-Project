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
//

/**
 * @brief This function pointer type is used to explore the maze. It is expected
 * to move the robot in the specified direction and return the walls that it
 * sees.
 *
 * @param p_grid Pointer to the maze.
 * @param p_navigator Pointer to the navigator state.
 * @param direction Direction to explore.
 *
 * @return int16_t Returns a bitmask of the walls.
 */
typedef uint16_t (*explore_func_t)(grid_t              *p_grid,
                                   navigator_state_t   *p_navigator,
                                   cardinal_direction_t direction);

/**
 * @brief Initialises a maze with no walls to perform the floodfill algorithm.
 *
 * @param p_grid Pointer to an uninitialised maze.
 */
void initialise_empty_maze_nowall(grid_t *p_grid);

/**
 * @brief Performs the floodfill algorithm to map the maze and determine a path
 * to the end.
 *
 * @param p_grid Pointer to the maze.
 * @param p_start_node Pointer to the starting node.
 * @param p_end_node Pointer to the destination node.
 * @param p_navigator Pointer to the navigator state.
 * @param p_explore_func Pointer to the function to explore the maze.
 */
void floodfill(grid_t            *p_grid,
               grid_cell_t       *p_start_node,
               grid_cell_t       *p_end_node,
               navigator_state_t *p_navigator,
               explore_func_t     p_explore_func);

#endif

// End of file pathfinding/floodfill.h
