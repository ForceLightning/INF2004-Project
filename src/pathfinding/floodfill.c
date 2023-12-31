/**
 * @file floodfill.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the implementation of the floodfill algorithm for
 * mapping the maze and determining a path to the end.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "pathfinding/maze.h"
#include "pathfinding/binary_heap.h"
#include "pathfinding/floodfill.h"

// Private function prototypes.
// ----------------------------------------------------------------------------
//

static void floodfill(binary_heap_t          *p_open_set,
                      maze_navigator_state_t *p_navigator);

// Public function definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief This function initialises a maze with no walls to perform the
 * floodfill algorithm on.
 *
 * @param[in,out] p_grid Pointer to the maze.
 */
void
floodfill_init_maze_nowall (maze_grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->f           = 0;
            p_cell->g           = 0;
            p_cell->h           = 0;
            p_cell->coordinates = (maze_point_t) { col, row };
            p_cell->p_came_from = NULL;
            p_cell->is_visited  = false;

            for (int16_t i = 0; 4 > i; i++)
            {
                maze_grid_cell_t *p_neighbour
                    = maze_get_cell_in_dir(p_grid, p_cell, i);

                if (NULL != p_neighbour)
                {
                    p_cell->p_next[i] = p_neighbour;
                }
                else
                {
                    p_cell->p_next[i] = NULL;
                }
            }
        }
    }
}

/**
 * @brief Runs the floodfill algorithm to map out the maze.
 *
 * @param[in,out] p_grid Pointer to the initialised maze with no walls.
 * @param[in] p_end_node Pointer to the end node.
 * @param[in,out] p_navigator Pointer to the navigator state.
 * @param[in] p_explore_func Pointer to the function that will explore the maze.
 * @param[in] p_move_navigator Pointer to the function that will move the
 * navigator.
 */
void
floodfill_map_maze (maze_grid_t               *p_grid,
                    const maze_grid_cell_t    *p_end_node,
                    maze_navigator_state_t    *p_navigator,
                    floodfill_explore_func_t   p_explore_func,
                    floodfill_move_navigator_t p_move_navigator)
{
    // Initialise the flood array.
    //

    // Like in A*, initialise the f, g, and h values of all nodes.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];

            p_cell->h = UINT32_MAX;
            p_cell->g = UINT32_MAX;
            p_cell->f = UINT32_MAX;
        }
    }

    // Start the inner loop.
    //
    while (p_navigator->p_current_node != p_end_node)
    {
        // Explore the current node.
        //
        p_explore_func(p_grid, p_navigator, p_navigator->orientation);

        // ! Possible to allocate this out of the loop?
        binary_heap_t flood_array;
        flood_array.p_array  = malloc(sizeof(binary_heap_node_t) * p_grid->rows
                                     * p_grid->columns);
        flood_array.capacity = p_grid->rows * p_grid->columns;
        flood_array.size     = 0;
        floodfill(&flood_array, p_navigator);
        // Get the next node to explore.
        //
        maze_grid_cell_t         *p_next_node = NULL;
        maze_cardinal_direction_t direction   = MAZE_NONE;

        for (uint8_t i = 0; 4 > i; i++)
        {
            maze_grid_cell_t *p_neighbour
                = p_navigator->p_current_node->p_next[i];

            if (NULL == p_neighbour)
            {
                continue;
            }

            if (p_neighbour->h < p_navigator->p_current_node->h)
            {
                p_next_node = p_neighbour;
                direction   = i;
                break;
            }
        }

        if (NULL == p_next_node)
        {
            // We have reached a dead end. We need to backtrack.
            //
            direction = (p_navigator->orientation + 2) % 4;
        }

        // Move the robot to the next node.
        //
        p_move_navigator(p_navigator, direction);
        maze_clear_heuristics(p_grid);
        // Free the flood array.
        //
        free(flood_array.p_array);
    }
}

// Private Functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Runs the floodfill algorithm to produce h-values for all nodes. Runs
 * every time the robot moves.
 *
 * @param[in,out] p_open_set Pointer to the open set.
 * @param[in,out] p_navigator Pointer to the navigator state.
 */
static void
floodfill (binary_heap_t *p_open_set, maze_navigator_state_t *p_navigator)
{
    // First, update the flood array from the end node. We only update the h
    // value here.
    //
    maze_grid_cell_t *p_flood_node = p_navigator->p_end_node;
    p_flood_node->h                = 0;

    // Insert the start node into the flood array.
    //
    binary_heap_insert(p_open_set, p_flood_node, p_flood_node->h);

    // This should look similar to the A* algorithm except we are conditioning
    // on the h-value.
    //
    while (0 < p_open_set->size)
    {
        binary_heap_node_t p_current_node = binary_heap_peek(p_open_set);

        if (p_current_node.p_maze_node == p_navigator->p_current_node)
        {
            return;
        }

        // Remove the current node from the open set.
        //
        binary_heap_delete_min(p_open_set);

        for (uint8_t neighbour = 0; 4 > neighbour; neighbour++)
        {
            maze_grid_cell_t *p_neighbour
                = p_current_node.p_maze_node->p_next[neighbour];

            if (NULL == p_neighbour)
            {
                continue;
            }

            uint32_t tentative_h_score = p_current_node.p_maze_node->h + 1;
            maze_grid_cell_t *p_neighbour_node
                = p_current_node.p_maze_node->p_next[neighbour];

            if (tentative_h_score < p_neighbour_node->h)
            {
                p_neighbour_node->h = tentative_h_score;

                uint16_t neighbour_index
                    = binary_heap_get_node_idx(p_open_set, p_neighbour_node);

                if (UINT16_MAX == neighbour_index)
                {
                    uint32_t neighbour_priority = p_neighbour_node->h;
                    binary_heap_insert(
                        p_open_set, p_neighbour_node, neighbour_priority);
                }
                else
                {
                    p_open_set->p_array[neighbour_index].priority
                        = p_neighbour_node->h;
                    binary_heapify_up(p_open_set, neighbour_index);
                }

                p_neighbour_node->p_came_from = p_current_node.p_maze_node;
            }
        }
    }
}

// End of file pathfinding/floodfill.c
