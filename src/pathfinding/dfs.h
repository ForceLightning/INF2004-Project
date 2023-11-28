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
#include "pathfinding/maze.h"
#include "pathfinding/floodfill.h"

// Public function prototypes.
// ----------------------------------------------------------------------------
//

void dfs_depth_first_search(maze_grid_t               *p_grid,
                            maze_grid_cell_t          *p_start_node,
                            maze_navigator_state_t    *p_navigator,
                            floodfill_explore_func_t   p_explore_func,
                            floodfill_move_navigator_t p_move_navigator);

bool dfs_is_all_reachable_visited(maze_grid_t            *p_grid,
                                  maze_navigator_state_t *p_navigator);

#endif // DFS_H

/*** End of file main/pathfinding/dfs.h ***/
