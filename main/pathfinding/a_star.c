#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "math.h"

#include "binary_heap.h"
#include "a_star.h"

// Private function prototypes.
//
static uint32_t square_euclidean_norm(point_t *point_a, point_t *point_b);
void a_star(grid_t grid, grid_cell_t *p_start_node, grid_cell_t *p_end_node);
static void a_star_inner_loop(binary_heap_t *open_set, grid_cell_t *p_end_node);

/**
 * @brief This function calculates the square of the Euclidean norm between two
 * points. This is used as the heuristic and avoids the costly square root
 * operation.
 *
 * @param point_a First point.
 * @param point_b Second point.
 * @return uint32_t The square of the Euclidean norm.
 */
static uint32_t
square_euclidean_norm (point_t *point_a, point_t *point_b)
{
    // Casting from uint16_t to int and back to uint32_t is necessary to avoid
    // overflow. There is no risk of overflow when casting from uint16_t to int
    // because the maximum value of a uint16_t < INT_MAX.
    //
    uint32_t x_diff = abs(point_a->x - point_b->x);
    uint32_t y_diff = abs(point_a->y - point_b->y);

    return x_diff * x_diff + y_diff * y_diff;
}

static void
a_star_inner_loop (binary_heap_t *open_set, grid_cell_t *p_end_node)
{
    while (open_set->size > 0)
    {
        // Step 1: Get the node with the lowest F-value from the open set. If it
        // is the end node, return.
        heap_node_t p_current_node = peek(open_set);
        if (p_current_node.p_maze_node == p_end_node)
        {
            return;
        }

        delete_min(open_set);

        for (uint8_t neighbour = 4; 0 < neighbour; neighbour--)
        {
            // Step 2: Ensure that the neighbour is not NULL.
            //
            if (NULL == p_current_node.p_maze_node->p_next[neighbour])
            {
                continue;
            }

            // Step 3: Calculate the tentative g-score.
            //
            uint32_t tentative_g_score = p_current_node.p_maze_node->g + 1;

            if (tentative_g_score < p_current_node.p_maze_node->g)
            {
                // Step 4: Update the g-score and h-score of the neighbour,
                // since it is better than the previous value.
                //
                p_current_node.p_maze_node->g = tentative_g_score;
                p_current_node.p_maze_node->h = square_euclidean_norm(
                    &p_current_node.p_maze_node->coordinates,
                    &p_end_node->coordinates);

                // Step 5: Check if the neighbour is in the open set. If not,
                // add it.
                //
                if (NULL
                    == p_current_node.p_maze_node->p_next[neighbour]
                           ->p_came_from)
                {
                    uint32_t neighbour_priority
                        = p_current_node.p_maze_node->g
                          + p_current_node.p_maze_node->h;
                    insert(open_set,
                           p_current_node.p_maze_node->p_next[neighbour],
                           neighbour_priority);
                }
                p_current_node.p_maze_node->p_next[neighbour]->p_came_from
                    = p_current_node.p_maze_node;
            }
        }
    }
}

/**
 * @brief Runs the A* algorithm on a grid maze to find the shortest path between
 * two points, if one exists. The path can be retrieved by checking the
 * `p_came_from` field of each node.
 *
 * @param grid The grid maze.
 * @param p_start_node Pointer to the start node.
 * @param p_end_node Pointer to the end node.
 */
void
a_star (grid_t grid, grid_cell_t *p_start_node, grid_cell_t *p_end_node)
{
    // Step 1: Initialise the open set heap.
    //
    binary_heap_t open_set;
    open_set.p_array  = malloc(sizeof(heap_node_t) * grid.rows * grid.columns);
    open_set.capacity = grid.rows * grid.columns;
    open_set.size     = 0;

    // Step 2: Initialise g-values and h-values of all nodes to UINT16_MAX.
    //
    for (uint16_t row = grid.rows; 0 < row; row--)
    {
        for (uint16_t col = grid.columns; 0 < col; col--)
        {
            grid_cell_t *p_cell = &grid.p_grid_array[row * grid.columns + col];
            p_cell->g           = UINT16_MAX;
            p_cell->h           = UINT16_MAX;
        }
    }

    // Step 3: Insert the start node into the open set.
    //
    uint32_t start_node_priority = square_euclidean_norm(
        &p_start_node->coordinates, &p_end_node->coordinates);
    p_start_node->g = 0;
    insert(&open_set, p_start_node, start_node_priority);

    // Step 4: Run the inner loop.
    //
    a_star_inner_loop(&open_set, p_end_node);

    // Step 5: Clean up.
    free(open_set.p_array);
}

// End of file comment
