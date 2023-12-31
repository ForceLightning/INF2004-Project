/**
 * @file maze.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Header file for the maze data structure and public functions.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MAZE_H // Include guard.
#define MAZE_H
#include <stdint.h>
#include <stdbool.h>

// Definitions.
// ----------------------------------------------------------------------------
//

/**
 * @def MAZE_INVERT_BITMASK(x)
 * @brief This macro inverts an 4-bit bitmask.
 * @param[in] x Bitmask to invert.
 */
#define MAZE_INVERT_BITMASK(x) ((0xFu - x) & 0xFu)

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief This struct contains the coordinates of a point.
 *
 */
typedef struct maze_point
{
    uint16_t x; ///< X-coordinate or column.
    uint16_t y; ///< Y-coordinate or row.
} maze_point_t;

/**
 * @brief This enum contains the possible directions.
 *
 */
typedef enum
{
    MAZE_NORTH = 0,  ///< North is the top of the maze.
    MAZE_EAST  = 1,  ///< East is the right of the maze.
    MAZE_SOUTH = 2,  ///< South is the bottom of the maze.
    MAZE_WEST  = 3,  ///< West is the left of the maze.
    MAZE_NONE  = 255 ///< None is used for errors.
} maze_cardinal_direction_t;

/**
 * @brief This enum contains bitmasks for the walls. Useful when updating more
 * than one wall at a time.
 *
 * @note The bitmasks are powers of 2, so they can be bitwise ORed together.
 * They DIFFER from the @ref cardinal_direction_t enum.
 */
typedef enum
{
    MAZE_NO_WALLS   = 0, ///< No walls.
    MAZE_FRONT_WALL = 1, ///< Front wall.
    MAZE_RIGHT_WALL = 2, ///< Right wall.
    MAZE_BACK_WALL  = 4, ///< Back wall.
    MAZE_LEFT_WALL  = 8  ///< Left wall.
} maze_wall_direction_t;

/**
 * @brief This enum contains the relative directions of instructions from the
 * navigator's perspective.
 *
 */
typedef enum
{
    MAZE_FRONT, ///< Front is the direction the navigator is facing.
    MAZE_RIGHT, ///< Right of the navigator.
    MAZE_BACK,  ///< Back of the navigator.
    MAZE_LEFT   ///< Left of the navigator.
} maze_relative_direction_t;

/**
 * @brief This struct contains the node information.
 *
 * @note Ensure that the initialisation of the struct sets all pointers to
 * NULL. Otherwise, the program might have undefined behaviour.
 */
typedef struct maze_grid_cell
{
    maze_point_t coordinates; ///< X and Y coordinates of the node.
    uint32_t     f;           ///< F-value of the node. F = G + H.
    uint32_t     g; ///< G-value of the node. G = cost to move from the
                    ///< starting node to the current node.
    uint32_t h;     ///< H-value of the node. H = estimated cost to move
                    ///< from the current node to the end node aka
                    ///< heuristic.
    struct maze_grid_cell *p_next[4]; ///< Pointers to the next nodes. This is
                                      ///< indexed by the direction enum.
    struct maze_grid_cell
        *p_came_from; ///< Pointer to the node that the current
                      ///< node came from for the A* algorithm.
    bool is_visited;  ///< Indicates if the node has been visited before.
} maze_grid_cell_t;

/**
 * @brief This struct contains the maze grid information.
 *
 * @note The grid array is indexed by row first, then column.
 */
typedef struct maze_grid
{
    maze_grid_cell_t *p_grid_array; ///< Pointer to the first element
                                    ///< of the grid array.
    uint16_t rows;                  ///< Number of rows in the grid.
    uint16_t columns;               ///< Number of columns in the grid.
} maze_grid_t;

/**
 * @brief This struct contains the state of a navigator in the maze.
 */
typedef struct maze_navigator_state
{
    maze_grid_cell_t *p_current_node; ///< Pointer to the current
                                      ///< location of the navigator.
    maze_grid_cell_t *p_start_node; ///< Pointer to the start node of the maze.
    maze_grid_cell_t *p_end_node;   ///< Pointer to the end node of the maze.
    maze_cardinal_direction_t orientation; ///< Orientation of the navigator.
                                           ///< This is an enum.
} maze_navigator_state_t;

/**
 * @brief This struct contains the maze gap bitmasks for serialisation.
 */
typedef struct maze_gap_bitmask
{
    uint16_t *p_bitmask; ///< Pointer to the bitmask array. This is indexed by
                         ///< row first, then column.
    uint16_t rows;       ///< Number of rows.
    uint16_t columns;    ///< Number of columns.
} maze_gap_bitmask_t;

/**
 * @brief Used for sending the maze over serial or WiFi.
 */
typedef union maze_bitmask_compressed
{
    struct
    {
        uint8_t cell_b : 4; ///< Cell B bitmask (LSB).
        uint8_t cell_a : 4; ///< Cell A bitmask (MSB).
    } fields;               ///< Bit masks for each cell.
    uint8_t bits;           ///< Combined bitmasks.
} maze_bitmask_compressed_t;

// Public Functions.
// ----------------------------------------------------------------------------
//

maze_grid_t maze_create(uint16_t rows, uint16_t columns);

void maze_initialise_empty_walled(maze_grid_t *p_grid);

void maze_clear_heuristics(maze_grid_t *p_grid);

void maze_destroy(maze_grid_t *p_grid);

int8_t maze_get_nav_dir_offset(const maze_navigator_state_t *p_navigator);

void maze_nav_modify_walls(maze_grid_t            *p_grid,
                           maze_navigator_state_t *p_navigator,
                           uint8_t                 aligned_wall_bitmask,
                           bool                    is_set,
                           bool                    is_unset);

char *maze_get_string(maze_grid_t *p_grid);

void maze_insert_nav_str(const maze_grid_t            *p_grid,
                         const maze_navigator_state_t *p_navigator,
                         char                         *maze_str);

maze_cardinal_direction_t maze_get_dir_from_to(const maze_point_t *p_point_a,
                                               const maze_point_t *p_point_b);

int16_t maze_deserialise(maze_grid_t        *p_grid,
                         maze_gap_bitmask_t *p_no_walls_array);

maze_gap_bitmask_t maze_serialise(maze_grid_t *p_grid);

maze_grid_cell_t *maze_get_cell_at_coords(maze_grid_t        *p_grid,
                                          const maze_point_t *p_coordinates);

maze_grid_cell_t *maze_get_cell_in_dir(maze_grid_t              *p_grid,
                                       maze_grid_cell_t         *p_from,
                                       maze_cardinal_direction_t direction);

uint32_t maze_manhattan_dist(const maze_point_t *p_point_a,
                             const maze_point_t *p_point_b);

int16_t maze_serialised_to_buffer(const maze_gap_bitmask_t *p_bitmask,
                                  uint8_t                  *p_buffer,
                                  uint16_t                  buffer_size);

int16_t maze_nav_to_buffer(const maze_navigator_state_t *p_navigator,
                           uint8_t                      *p_buffer,
                           uint16_t                      buffer_size);

void maze_uint16_to_uint8_buffer(uint16_t value, uint8_t *p_buffer);

maze_relative_direction_t maze_get_relative_dir(
    const maze_cardinal_direction_t direction_from,
    const maze_cardinal_direction_t direction_to);

#endif // MAZE_H

// End of pathfinding/maze.h
