/**
 * @file a_star.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Header file for the a* algorithm.
 * @version 0.2
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef A_STAR_H // Include guard.
#define A_STAR_H

#include <stdint.h>
#include "pathfinding/binary_heap.h"
#include "pathfinding/maze.h"

#ifndef NDEBUG
/**
 * @def DEBUG_PRINT(...)
 * @brief Macro for printing debug messages.
 * @param ... Variable number of arguments to be printed.
 * @note This macro is only defined if NDEBUG is not defined.
 */
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Struct containing the path from the start node to the end node.
 * @see a_star_path
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

char *a_star_get_path_str(maze_grid_t *p_grid, a_star_path_t *p_path);

int16_t a_star_path_to_buffer(const a_star_path_t *p_path,
                              uint8_t             *p_buffer,
                              uint16_t             buffer_size);

int16_t a_star_maze_path_nav_to_buffer(
    maze_grid_t                  *p_grid,
    const a_star_path_t          *p_path,
    const maze_navigator_state_t *p_navigator,
    uint8_t                      *p_buffer,
    uint16_t                      buffer_size);

#endif // A_STAR_H

// End of pathfinding/a_star.h
