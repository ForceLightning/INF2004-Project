/**
 * @file a_star.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Header file for the a* algorithm.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef A_STAR_H
#define A_STAR_H

#include <stdint.h>
#include "binary_heap.h"
#include "maze.h"

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @typedef path_t
 * @brief Struct containing the path from the start node to the end node.
 *
 */
typedef struct a_star_path
{
    uint32_t          length; ///< Length of the path.
    maze_grid_cell_t *p_path; ///< Pointer to the first node in the path.
} a_star_path_t;

// Public functions.
// ----------------------------------------------------------------------------
//

void a_star(maze_grid_t      *p_grid,
            maze_grid_cell_t *p_start_node,
            maze_grid_cell_t *p_end_node);

a_star_path_t *a_star_get_path(maze_grid_cell_t *p_end_node);
char          *a_star_get_path_str(maze_grid_t *p_grid, a_star_path_t *p_path);
int16_t        a_star_path_to_buffer(a_star_path_t *p_path,
                                     uint8_t       *p_buffer,
                                     uint16_t       buffer_size);

#endif // A_STAR_H

// End of pathfinding/a_star.h
