/**
 * @file a_star.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Source file for the implementation of the a* algorithm.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "binary_heap.h"
#include "a_star.h"
#include "maze.h"

// Private function prototypes.
// ----------------------------------------------------------------------------
//

static void a_star_inner_loop(binary_heap_t    *p_open_set,
                              maze_grid_cell_t *p_end_node);
static void insert_path_directions(char                     *p_maze_string,
                                   maze_grid_cell_t         *p_cell,
                                   uint16_t                  str_num_cols,
                                   maze_cardinal_direction_t in_direction,
                                   maze_cardinal_direction_t out_direction);
static void insert_path_in_direction(char                     *p_maze_str,
                                     uint16_t                  str_num_cols,
                                     uint32_t                  node_row,
                                     uint32_t                  node_col,
                                     maze_cardinal_direction_t direction);
static void insert_node_centre_char(char    *p_maze_string,
                                    uint32_t row,
                                    uint32_t col,
                                    uint16_t str_num_cols,
                                    char     symbol);

// Public functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Runs the A* algorithm on a grid maze to find the shortest path between
 * two points, if one exists. The path can be retrieved by checking the
 * `p_came_from` field of each node.
 *
 * @param[in] p_grid The grid maze.
 * @param[in] p_start_node Pointer to the start node.
 * @param[in] p_end_node Pointer to the end node.
 */
void
a_star (maze_grid_t      *p_grid,
        maze_grid_cell_t *p_start_node,
        maze_grid_cell_t *p_end_node)
{
    // Step 1: Initialise the open set heap.
    //
    binary_heap_t open_set;
    open_set.p_array
        = malloc(sizeof(binary_heap_node_t) * p_grid->rows * p_grid->columns);
    open_set.capacity = p_grid->rows * p_grid->columns;
    open_set.size     = 0;

    // Step 2: Initialise g-values and h-values of all nodes to UINT16_MAX.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->g = UINT16_MAX;
            p_cell->h = UINT16_MAX;
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
 * @param p_end_node Pointer to the end node.
 * @return path_t* Pointer to the path.
 */
a_star_path_t *
a_star_get_path (maze_grid_cell_t *p_end_node)
{
    uint32_t          path_length    = 0;
    maze_grid_cell_t *p_current_node = p_end_node;
    path_length                      = p_current_node->p_came_from->g + 2;
    maze_grid_cell_t *p_path = malloc(sizeof(maze_grid_cell_t) * path_length);
    a_star_path_t    *p_path_struct = malloc(sizeof(a_star_path_t));
    p_path_struct->length           = path_length;
    p_path_struct->p_path           = p_path;

    // Traverse the path backwards and store it in the path array in reverse.
    //
    for (uint32_t reverse_index = path_length - 1; 0 < reverse_index;
         reverse_index--)
    {
        p_path[reverse_index] = *p_current_node;
        p_current_node        = p_current_node->p_came_from;
    }
    p_path[0] = *p_current_node;

    return p_path_struct;
}

/**
 * @brief Gets the string representation of the path from the start node to the
 * end node.
 *
 * @param p_grid Pointer to the grid maze.
 * @param p_path Pointer to the path.
 *
 * @return char* The string representation of the path.
 */
char *
a_star_get_path_str (maze_grid_t *p_grid, a_star_path_t *p_path)
{
    char *p_maze_string = get_maze_string(p_grid);

    // Initialise the pointers to the current and previous cells.
    //
    maze_grid_cell_t *p_cell
        = &p_path->p_path[p_path->length - 1];  // Current cell.
    maze_grid_cell_t *p_previous_cell = p_cell; // Previous cell.

    uint16_t str_num_cols
        = p_grid->columns * 4 + 2; // Number of columns in the string.

    // For each node, find the direction that leads in and out of the node, then
    // add to the string. Special cases are the start and end nodes. This
    // handles the end node.
    //
    maze_cardinal_direction_t direction = get_direction_from_to(
        &p_cell->coordinates, &p_cell->p_came_from->coordinates);

    insert_path_directions(
        p_maze_string, p_cell, str_num_cols, direction, NONE);

    // Now for the rest of the path.
    //
    for (uint32_t index = p_path->length - 2; 0 < index; index--)
    {
        // Find the direction that leads in and out of the current cell, then
        // add to the string.
        //
        p_previous_cell = p_cell;
        p_cell          = &p_path->p_path[index];

        maze_cardinal_direction_t in_direction = get_direction_from_to(
            &p_cell->coordinates, &p_cell->p_came_from->coordinates);
        maze_cardinal_direction_t out_direction = get_direction_from_to(
            &p_cell->coordinates, &p_previous_cell->coordinates);

        insert_path_directions(
            p_maze_string, p_cell, str_num_cols, in_direction, out_direction);
    }

    // Now for the start node.
    //
    p_previous_cell = p_cell;
    p_cell          = &p_path->p_path[0];

    direction = get_direction_from_to(&p_cell->coordinates,
                                      &p_previous_cell->coordinates);

    insert_path_directions(
        p_maze_string, p_cell, str_num_cols, NONE, direction);
    return p_maze_string;
}

// Private functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Contains the inner loop of the A* algorithm.
 *
 * @param p_open_set The open set heap which contains all unexplored nodes
 * adjacent to explored nodes.
 * @param p_end_node Pointer to the end node.
 *
 * @see https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
 */
static void
a_star_inner_loop (binary_heap_t *p_open_set, maze_grid_cell_t *p_end_node)
{
    while (p_open_set->size > 0)
    {
        // Step 1: Get the node with the lowest F-value from the open set. If it
        // is the end node, return.
        binary_heap_node_t p_current_node = peek(p_open_set);
        if (p_current_node.p_maze_node == p_end_node)
        {
            return;
        }

        delete_min(p_open_set);

        for (uint8_t neighbour = 0; 4 > neighbour; neighbour++)
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
            maze_grid_cell_t *p_neighbour_node
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
                    = get_index_of_node(p_open_set, p_neighbour_node);
                if (UINT16_MAX == neighbour_index)
                {
                    uint32_t neighbour_priority
                        = p_current_node.p_maze_node->g
                          + p_current_node.p_maze_node->h;

                    insert(p_open_set,
                           p_current_node.p_maze_node->p_next[neighbour],
                           neighbour_priority);
                }
                // Otherwise, update the priority of the neighbour.
                else
                {
                    p_open_set->p_array[neighbour_index].priority
                        = p_neighbour_node->f;
                    heapify_up(p_open_set, neighbour_index);
                }

                p_current_node.p_maze_node->p_next[neighbour]->p_came_from
                    = p_current_node.p_maze_node;
            }
        }
    }
}

/**
 * @brief Inserts the path directions and node centres into the maze string.
 *
 * @param p_maze_string Pointer to the maze string.
 * @param p_cell Pointer to the current cell.
 * @param str_num_cols Number of columns in the maze string.
 * @param in_direction Direction that leads into the current cell.
 * @param out_direction Direction that leads out of the current cell.
 */
static void
insert_path_directions (char                     *p_maze_string,
                        maze_grid_cell_t         *p_cell,
                        uint16_t                  str_num_cols,
                        maze_cardinal_direction_t in_direction,
                        maze_cardinal_direction_t out_direction)
{
    uint32_t node_row = p_cell->coordinates.y * 2 + 1;
    uint32_t node_col = p_cell->coordinates.x * 4 + 2;
    if (in_direction != NONE)
    {
        insert_path_in_direction(
            p_maze_string, str_num_cols, node_row, node_col, in_direction);
    }

    switch (in_direction)
    {
        case NONE:
            // Deals with the start node.
            insert_node_centre_char(
                p_maze_string, node_row, node_col, str_num_cols, '%');
            goto handle_end;
        case NORTH:
            if (SOUTH == out_direction)
            {
                insert_node_centre_char(
                    p_maze_string, node_row, node_col, str_num_cols, '|');
            }
            else
            {
                goto handle_direction_change;
            }
            goto handle_end;
        case EAST:
            if (WEST == out_direction)
            {
                insert_node_centre_char(
                    p_maze_string, node_row, node_col, str_num_cols, '-');
            }
            else
            {
                goto handle_direction_change;
            }
            goto handle_end;
        case SOUTH:
            if (NORTH == out_direction)
            {
                insert_node_centre_char(
                    p_maze_string, node_row, node_col, str_num_cols, '|');
            }
            else
            {
                goto handle_direction_change;
            }
            goto handle_end;
        case WEST:
            if (EAST == out_direction)
            {
                insert_node_centre_char(
                    p_maze_string, node_row, node_col, str_num_cols, '-');
            }
            else
            {
                goto handle_direction_change;
            }
            goto handle_end;
    }
handle_direction_change:
    insert_node_centre_char(
        p_maze_string, node_row, node_col, str_num_cols, 'O');
handle_end:
    if (NONE == out_direction)
    {
        // Deals with the end node.
        insert_node_centre_char(
            p_maze_string, node_row, node_col, str_num_cols, 'X');
    }
}

/**
 * @brief Inserts the path string in the given direction.
 *
 * @param p_maze_str Pointer to the string containing the pretty printed maze.
 * @param str_num_cols Number of rows in the string.
 * @param node_row Row of the node to add the path to.
 * @param node_col Column of the node to add the path to.
 * @param direction Direction to add the path in.
 */
static void
insert_path_in_direction (char                     *p_maze_str,
                          uint16_t                  str_num_cols,
                          uint32_t                  node_row,
                          uint32_t                  node_col,
                          maze_cardinal_direction_t direction)
{

    switch (direction)
    {
        case NORTH:
            p_maze_str[(node_row - 1) * str_num_cols + node_col] = '|';
            break;
        case EAST:
            for (uint32_t index = 0; 3 > index; index++)
            {
                p_maze_str[node_row * str_num_cols + node_col + index + 1]
                    = '-';
            }
            break;
        case SOUTH:
            p_maze_str[(node_row + 1) * str_num_cols + node_col] = '|';
            break;
        case WEST:
            for (uint32_t index = 0; 3 > index; index++)
            {
                p_maze_str[node_row * str_num_cols + node_col + index - 3]
                    = '-';
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Inserts the symbol into the centre of the node's cell for printing.
 *
 * @param p_maze_string Pointer to the maze string.
 * @param row Row of the node in coordinates of the maze string.
 * @param col Column of the node in coordinates of the maze string.
 * @param symbol Symbol to insert.
 */
static void
insert_node_centre_char (char    *p_maze_string,
                         uint32_t row,
                         uint32_t col,
                         uint16_t str_num_cols,
                         char     symbol)
{
    p_maze_string[row * str_num_cols + col] = symbol;
}

// End of pathfinding/a_star.c
