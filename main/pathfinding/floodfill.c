#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "maze.h"
#include "binary_heap.h"
#include "floodfill.h"

// Private function prototypes.
//
static void floodfill_inner_loop(grid_t            *p_grid,
                                 navigator_state_t *p_navigator,
                                 explore_func_t     p_explore_func);

/**
 * @brief This function initialises a maze with no walls to perform the
 * floodfill algorithm on.
 *
 * @param p_grid Pointer to the maze.
 */
void
initialise_empty_maze_nowall (grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->f           = 0;
            p_cell->g           = 0;
            p_cell->h           = 0;
            p_cell->coordinates = (point_t) { col, row };
            p_cell->p_came_from = NULL;

            for (int16_t i = 0; 4 > i; i++)
            {
                grid_cell_t *p_neighbour
                    = get_cell_in_direction(p_grid, p_cell, i);

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
 * @param p_grid Pointer to the initialised maze with no walls.
 * @param p_start_node Pointer to the start node.
 * @param p_end_node Pointer to the end node.
 * @param p_explore_func Pointer to the function that will explore the maze.
 */
void
floodfill (grid_t            *p_grid,
           grid_cell_t       *p_start_node,
           grid_cell_t       *p_end_node,
           navigator_state_t *p_navigator,
           explore_func_t     p_explore_func)
{
    // Initialise the flood array.
    //
    binary_heap_t flood_array;
    flood_array.p_array
        = malloc(sizeof(heap_node_t) * p_grid->rows * p_grid->columns);
    flood_array.capacity = p_grid->rows * p_grid->columns;
    flood_array.size     = 0;

    grid_cell_t *p_current_node = p_start_node;

    // Like in A*, initialise the f, g, and h values of all nodes.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];

            // May only need to initialise either the f or g value.
            p_cell->g = manhattan_distance(&p_cell->coordinates,
                                           &p_start_node->coordinates);
            p_cell->f = p_cell->g;
        }
    }

    // Insert the start node into the flood array.
    //
    insert(&flood_array, p_start_node, p_start_node->f);

    // Start the inner loop.
    //
    while (p_current_node != p_end_node)
    {
        floodfill_inner_loop(p_grid, p_navigator, p_explore_func);
    }
}

static void
floodfill_inner_loop (grid_t            *p_grid,
                      navigator_state_t *p_navigator,
                      explore_func_t     p_explore_func)
{
    // Find the next node to explore. It will be the node with the lowest f
    // value.
    //
    uint32_t     lowest_f             = UINT32_MAX;
    grid_cell_t *p_next_node          = NULL;
    int16_t      direction_to_explore = 0;
    grid_cell_t *p_current_node       = p_navigator->p_current_node;

    // Check left, forward and right directions.
    for (int16_t direction = 0; 4 > direction; direction++)
    {
        // Ignore the back direction.
        if (direction == 2)
        {
            continue;
        }

        int8_t cardinal_offset = get_offset_from_nav_direction(p_navigator);
        // Conversion from int8_t to int16_t is safe.
        //
        direction_to_explore = (direction - (int16_t)cardinal_offset) % 4;

        grid_cell_t *p_neighbour = get_cell_in_direction(
            p_grid, p_current_node, direction_to_explore);

        // Set the neighbour to explore if it is not NULL.
        //
        if (NULL != p_neighbour)
        {
            if (lowest_f > p_neighbour->f)
            {
                lowest_f    = p_neighbour->f;
                p_next_node = p_neighbour;
            }
        }
    }

    // Explore the neighbour.
    //
    if (NULL != p_next_node)
    {
        uint16_t wall_bitmask
            = p_explore_func(p_grid, p_navigator, direction_to_explore);
        navigator_modify_walls(p_grid, p_navigator, wall_bitmask, true, false);
    }

    // Update all the f values of the neighbours.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            // TODO(chris): Calculate the distances starting from the end node
            // to every other node.
        }
    }
}