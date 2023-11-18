/**
 * @file dfs.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Header file for the depth first search algorithm for mapping the maze.
 * @version 0.1
 * @date 2023-11-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef DFS_H // Include guard.
#define DFS_H

#include <stdint.h>
#include "maze.h"
#include "floodfill.h"

void dfs_depth_first_search(grid_t            *p_grid,
                            grid_cell_t       *p_start_node,
                            navigator_state_t *p_navigator,
                            explore_func_t     p_explore_func,
                            move_navigator_t   p_move_navigator);

bool dfs_is_all_visited(grid_t *p_grid);
bool dfs_is_all_reachable_visited(grid_t            *p_grid,
                                  navigator_state_t *p_navigator);

#endif // DFS_H

/*** End of file main/pathfinding/dfs.h ***/
