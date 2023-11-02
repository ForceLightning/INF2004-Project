#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary_heap.h"
#include "a_star.h"
#include "maze.h"

// Private function prototypes.
//
static void a_star_inner_loop(binary_heap_t *open_set, grid_cell_t *p_end_node);

/**
 * @brief Calculates the manhattan distance between two points.
 *
 * @param point_a First point.
 * @param point_b Second point.
 * @return uint32_t The manhattan distance.
 *
 * @ref https://en.wikipedia.org/wiki/Taxicab_geometry
 */
uint32_t
manhattan_distance (point_t *point_a, point_t *point_b)
{
    uint32_t x_diff = abs(point_a->x - point_b->x);
    uint32_t y_diff = abs(point_a->y - point_b->y);
    return x_diff + y_diff;
}

/**
 * @brief Contains the inner loop of the A* algorithm.
 *
 * @param open_set The open set heap which contains all unexplored nodes
 * adjacent to explored nodes.
 * @param p_end_node Pointer to the end node.
 *
 * @ref https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
 */
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

        for (uint8_t neighbour = 0; 4 > neighbour; neighbour--)
        {
            // Step 2: Ensure that the neighbour is not NULL.
            //
            if (NULL == p_current_node.p_maze_node->p_next[neighbour])
            {
                continue;
            }

            // Step 3: Calculate the tentative g-score.
            //
            uint32_t     tentative_g_score = p_current_node.p_maze_node->g + 1;
            grid_cell_t *p_neighbour_node
                = p_current_node.p_maze_node->p_next[neighbour];

            if (tentative_g_score < p_neighbour_node->g)
            {
                // Step 4: Update the g-score and h-score of the neighbour,
                // since it is better than the previous value.
                //
                p_neighbour_node->g = tentative_g_score;
                p_neighbour_node->h = manhattan_distance(
                    &p_current_node.p_maze_node->coordinates,
                    &p_end_node->coordinates);
                p_neighbour_node->f = p_neighbour_node->g + p_neighbour_node->h;

                // Step 5: Check if the neighbour is in the open set. If not,
                // add it.
                //
                uint16_t neighbour_index
                    = get_index_of_node(open_set, p_neighbour_node);
                if (UINT16_MAX == neighbour_index)
                {
                    uint32_t neighbour_priority
                        = p_current_node.p_maze_node->g
                          + p_current_node.p_maze_node->h;

                    insert(open_set,
                           p_current_node.p_maze_node->p_next[neighbour],
                           neighbour_priority);
                }
                // Otherwise, update the priority of the neighbour.
                else
                {
                    open_set->p_array[neighbour_index].priority
                        = p_neighbour_node->f;
                    heapify_up(open_set, neighbour_index);
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
    for (int16_t row = grid.rows - 1; 0 <= row; row--)
    {
        for (int16_t col = grid.columns - 1; 0 <= col; col--)
        {
            grid_cell_t *p_cell = &grid.p_grid_array[row * grid.columns + col];
            p_cell->g           = UINT16_MAX;
            p_cell->h           = UINT16_MAX;
        }
    }

    // Step 3: Insert the start node into the open set.
    //
    uint32_t start_node_priority = manhattan_distance(
        &p_start_node->coordinates, &p_end_node->coordinates);
    p_start_node->g = 0;
    p_start_node->h = start_node_priority;
    p_start_node->f = start_node_priority;
    insert(&open_set, p_start_node, start_node_priority);

    // Step 4: Run the inner loop.
    //
    a_star_inner_loop(&open_set, p_end_node);

    // Step 5: Clean up.
    free(open_set.p_array);
}

/**
 * @brief Get the path from the start node to the end node.
 *
 * @param p_grid
 * @param p_start_node
 * @param p_end_node
 * @return grid_cell_t*
 */
grid_cell_t *
get_path (grid_cell_t *p_end_node)
{
    uint32_t     path_length    = 0;
    grid_cell_t *p_current_node = p_end_node;
    path_length                 = p_current_node->p_came_from->g + 2;
    grid_cell_t *p_path         = malloc(sizeof(grid_cell_t) * path_length);

    // Traverse the path backwards and store it in the path array in reverse.
    //
    for (uint32_t reverse_index = path_length - 1; 0 < reverse_index;
         reverse_index--)
    {
        p_path[reverse_index] = *p_current_node;
        p_current_node        = p_current_node->p_came_from;
    }
    p_path[0] = *p_current_node;

    return p_path;
}

// End of file comment
