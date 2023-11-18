#ifndef DFS_H
#define DFS_H

#include <stdint.h>
#include "maze.h"
#include "floodfill.h"

void dfs_depth_first_search(grid_t            *p_grid,
                            grid_cell_t       *p_start_node,
                            navigator_state_t *p_navigator,
                            explore_func_t     explore_func,
                            move_navigator_t   move_navigator);

bool dfs_is_all_visited(grid_t *p_grid);
bool dfs_is_all_reachable_visited(grid_t            *p_grid,
                                     navigator_state_t *p_navigator);

#endif // DFS_H

// End of file main/pathfinding/dfs.h