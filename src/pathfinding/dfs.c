/**
 * @file dfs.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Runs the depth first search algorithm on a maze.
 * @version 0.1
 * @date 2023-11-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "maze.h"
#include "floodfill.h"
#include "binary_heap.h"
#include "dfs.h"

// Private function prototypes.
//
static void reachable_floodfill(binary_heap_t          *p_reachable_set,
                                maze_navigator_state_t *p_navigator);

/**
 * @brief Conducts a depth first search on a maze.
 *
 * @param[in] p_grid Pointer to the grid.
 * @param[in] p_start_node Pointer to the start node.
 * @param[in,out] p_navigator Pointer to the navigator state.
 * @param[in] p_explore_func Function pointer to explore the current node.
 * @param[in] p_move_navigator Function pointer to move the navigator.
 */
void
dfs_depth_first_search (maze_grid_t               *p_grid,
                        maze_grid_cell_t          *p_start_node,
                        maze_navigator_state_t    *p_navigator,
                        floodfill_explore_func_t   p_explore_func,
                        floodfill_move_navigator_t p_move_navigator)
{
    // Step 1: Initialise is_visited to false.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->is_visited = false;
        }
    }
    p_start_node->is_visited = true;

    // Step 2: Get the next node to explore.
    //
    const maze_grid_cell_t   *p_next_node = NULL;
    maze_cardinal_direction_t direction   = MAZE_NONE;

    while (!dfs_is_all_reachable_visited(p_grid, p_navigator))
    {
        // Step 3: Explore the current node
        //
        uint16_t gap_bitmask
            = p_explore_func(p_grid, p_navigator, p_navigator->orientation);
        maze_nav_modify_walls(p_grid, p_navigator, gap_bitmask, true, false);

        for (uint8_t direction_idx = 0; 4 > direction_idx; direction_idx++)
        {
            maze_grid_cell_t *p_neighbour
                = p_navigator->p_current_node->p_next[direction_idx];

            // Step 4: Check if the neighbour is NULL or visited. We cannot
            // check both in the same if statement because of short circuiting.
            //
            if (NULL == p_neighbour)
            {
                continue;
            }

            if (p_neighbour->is_visited)
            {
                continue;
            }

            // Step 5: If the neighbour is not NULL or visited, set it as the
            // next node.
            //
            p_next_node = p_neighbour;
            direction   = direction_idx;
            break;
        }

        // Step 6: Check if it is a dead end.
        //
        if (NULL == p_next_node)
        {
            // We have reached a dead end, backtrack.
            //
            p_next_node = p_navigator->p_current_node->p_came_from;
            direction   = maze_get_dir_from_to(
                &p_navigator->p_current_node->coordinates,
                &p_next_node->coordinates);
        }

        // Step 7: Move the robot to the next node.
        //
        p_move_navigator(p_navigator, direction);
        p_next_node = NULL;
    }
}

/**
 * @brief Checks if all reachable nodes from the navigator's current position
 * has been visisted.
 *
 * @param[in] p_grid Pointer to the grid.
 * @param[in] p_navigator Pointer to the navigator state.
 * @return true All reachable nodes have been visited.
 * @return false Not all reachable nodes have been visited.
 */
bool
dfs_is_all_reachable_visited (maze_grid_t            *p_grid,
                              maze_navigator_state_t *p_navigator)
{
    // Step 1: Declare the reachable set.
    //
    binary_heap_t reachable_set;
    reachable_set.p_array
        = malloc(sizeof(binary_heap_node_t) * p_grid->rows * p_grid->columns);
    memset(reachable_set.p_array,
           0,
           sizeof(binary_heap_node_t) * p_grid->rows * p_grid->columns);
    reachable_set.capacity = p_grid->rows * p_grid->columns;
    reachable_set.size     = 0;

    // Step 2: Initialise the f, g, and h values of all nodes.
    for (size_t row = 0; p_grid->rows > row; row++)
    {
        for (size_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->g = UINT32_MAX;
            p_cell->h = UINT32_MAX;
            p_cell->f = UINT32_MAX;
        }
    }

    // Step 2: Set the g-score of the current node to 0.
    //
    maze_grid_cell_t *p_next_node = p_navigator->p_current_node;
    p_next_node->g                = 0;

    // Step 3: Conduct the floodfill.
    //
    reachable_floodfill(&reachable_set, p_navigator);

    // Step 4: Check if all the nodes in the reachable set have been visited.
    //
    bool is_visited = true;
    while (0 < reachable_set.size)
    {
        const maze_grid_cell_t *p_current_node
            = binary_heap_peek(&reachable_set).p_maze_node;
        is_visited &= p_current_node->is_visited;
        binary_heap_delete_min(&reachable_set);
        // Step 5: If not visited, end early and return false.
        //
        if (!is_visited)
        {
            goto end;
        }
    }
end:
    free(reachable_set.p_array);
    return is_visited;
}

// Private function definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Performs floodfill to retrieve all the reachable nodes.
 *
 * @param[out] p_reachable_set Pointer to the reachable set.
 * @param[in] p_navigator Pointer to the navigator state.
 */
static void
reachable_floodfill (binary_heap_t          *p_reachable_set,
                     maze_navigator_state_t *p_navigator)
{
    // Step 1: Declare the open set.
    //
    binary_heap_t open_set
        = { .p_array = NULL, .size = 0, .capacity = p_reachable_set->capacity };
    open_set.p_array
        = malloc(sizeof(binary_heap_node_t) * p_reachable_set->capacity);
    memset(open_set.p_array,
           0,
           sizeof(binary_heap_node_t) * p_reachable_set->capacity);

    // Step 2: Add the current node to the open set.
    //
    maze_grid_cell_t *p_next_node = p_navigator->p_current_node;
    binary_heap_insert(&open_set, p_next_node, p_next_node->h);

    while (0 < open_set.size)
    {
        binary_heap_node_t p_current_node = binary_heap_peek(&open_set);

        binary_heap_delete_min(&open_set);

        for (uint8_t neighbour_dir = 0; 4 > neighbour_dir; neighbour_dir++)
        {
            // Step 3: Ensure that the neighbour is not null.
            //
            maze_grid_cell_t *p_neighbour
                = p_current_node.p_maze_node->p_next[neighbour_dir];
            if (NULL == p_neighbour)
            {
                continue;
            }

            // Step 4: Ighnore neighbour g-scores that are smaller than the
            // current g-score.
            //
            uint32_t tentative_g_score = p_current_node.p_maze_node->g + 1;

            if (p_neighbour->g < p_current_node.p_maze_node->g)
            {
                continue;
            }

            // Step 5: Ensure that the neighbour is not in the reachable set,
            // then add it to the reachable set.
            //
            uint16_t neighbour_index
                = binary_heap_get_node_idx(p_reachable_set, p_neighbour);
            p_neighbour->g = tentative_g_score;

            if (UINT16_MAX == neighbour_index)
            {
                binary_heap_insert(
                    p_reachable_set, p_neighbour, p_neighbour->g);
            }

            // Step 6: Ensure that the neighbour is not in the open set, then
            // add it to the open set.
            //
            neighbour_index = binary_heap_get_node_idx(&open_set, p_neighbour);

            if (UINT16_MAX == neighbour_index)
            {
                binary_heap_insert(&open_set, p_neighbour, p_neighbour->g);
            }
        }
    }

    free(open_set.p_array);
}
// Private functions definitions
// ----------------------------------------------------------------------------
//

// End of file main/pathfinding/dfs.c