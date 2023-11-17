#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary_heap.h"
#include "maze.h"
#include "floodfill.h"
#include "dfs.h"

// Private function prototypes.
//
static bool is_all_visited(grid_t *p_grid);

void
dfs_depth_first_search (grid_t            *p_grid,
                        grid_cell_t       *p_start_node,
                        navigator_state_t *p_navigator,
                        explore_func_t     explore_func,
                        move_navigator_t   move_navigator)
{
    // Step 1: Initialise is_visited to false.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->is_visited = false;
        }
    }
    p_start_node->is_visited = true;

    // Get the next node to explore.
    //
    grid_cell_t         *p_next_node = NULL;
    cardinal_direction_t direction   = NONE;

    while (!is_all_visited(p_grid))
    {
        // Explore the current node
        //
        explore_func(p_grid, p_navigator, p_navigator->orientation);
        for (uint8_t direction_idx = 0; 4 > direction_idx; direction_idx++)
        {
            grid_cell_t *p_neighbour
                = p_navigator->p_current_node->p_next[direction_idx];
            if (NULL == p_neighbour)
            {
                continue;
            }
            else if (p_neighbour->is_visited)
            {
                continue;
            }

            if (!p_neighbour->is_visited)
            {
                p_next_node = p_neighbour;
                direction   = direction_idx;
                break;
            }
        }
        if (NULL == p_next_node)
        {
            // We have reached a dead end, backtrack.
            //
            p_next_node = p_navigator->p_current_node->p_came_from;
            direction   = get_direction_from_to(
                &p_navigator->p_current_node->coordinates,
                &p_next_node->coordinates);
        }

        // Move the robot to the next node.
        //
        move_navigator(p_navigator, direction);
        p_next_node = NULL;
    }
}

// Private functions definition.
//

static bool
is_all_visited (grid_t *p_grid)
{
    bool is_visited = true;
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_current_node
                = &p_grid->p_grid_array[p_grid->columns * row + col];
            is_visited &= p_current_node->is_visited;
            if (!is_visited)
            {
                goto end;
            }
        }
    }
end:
    return is_visited;
}
// End of file main/pathfinding/dfs.c