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

#ifndef MAZE_H
#define MAZE_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @typedef point_t
 * @brief This struct contains the coordinates of a point.
 *
 * @param x X-coordinate.
 * @param y Y-coordinate.
 */
typedef struct point
{

    uint16_t x; ///< X-coordinate or column.
    uint16_t y; ///< Y-coordinate or row.
} point_t;

/**
 * @typedef cardinal_direction_t
 * @brief This enum contains the possible directions.
 */
typedef enum cardinal_direction
{
    NORTH = 0,  ///< North is the top of the maze.
    EAST  = 1,  ///< East is the right of the maze.
    SOUTH = 2,  ///< South is the bottom of the maze.
    WEST  = 3,  ///< West is the left of the maze.
    NONE  = 255 ///< None is used for errors.
} cardinal_direction_t;

/**
 * @typedef wall_direction_t
 * @brief This enum contains bitmasks for the walls. Useful when updating more
 * than one wall at a time.
 *
 * @NOTE: The bitmasks are powers of 2, so they can be bitwise ORed together.
 * They DIFFER from the cardinal_direction_t enum.
 */
typedef enum wall_direction
{
    NO_WALLS   = 0, ///< No walls.
    FRONT_WALL = 1, ///< Front wall.
    RIGHT_WALL = 2, ///< Right wall.
    BACK_WALL  = 4, ///< Back wall.
    LEFT_WALL  = 8  ///< Left wall.
} wall_direction_t;

/**
 * @typedef grid_cell_t
 * @brief This struct contains the node information.
 *
 * @param coordinates X and Y coordinates of the node.
 * @param f F-value of the node. F = G + H.
 * @param g G-value of the node. G = cost to move from the starting node to the
 * current node.
 * @param h H-value of the node. H = estimated cost to move from the current
 * node to the end node aka heuristic.
 * @param p_parent Pointer to the parent node.
 * @param p_next Pointers to the next nodes. This is indexed by the direction
 * enum.
 * @param p_came_from Pointer to the node that the current node came from for
 * the A* algorithm.
 *
 * @NOTE: Ensure that the initialisation of the struct sets all pointers to
 * NULL. Otherwise, the program might have undefined behaviour.
 */
typedef struct grid_cell
{
    point_t  coordinates; ///< X and Y coordinates of the node.
    uint32_t f;           ///< F-value of the node. F = G + H.
    uint32_t g;           ///< G-value of the node. G = cost to move from the
                          ///< starting node to the current node.
    uint32_t h;           ///< H-value of the node. H = estimated cost to move
                          ///< from the current node to the end node aka
                          ///< heuristic.
    struct grid_cell *p_next[4];   ///< Pointers to the next nodes. This is
                                   ///< indexed by the direction enum.
    struct grid_cell *p_came_from; ///< Pointer to the node that the current
                                   ///< node came from for the A* algorithm.
} grid_cell_t;

/**
 * @typedef grid_t
 * @brief This struct contains the maze grid information.
 *
 * @param p_grid_array Pointer to the first element of the grid array.
 * @param rows Number of rows in the grid.
 * @param columns Number of columns in the grid.
 *
 * @NOTE: The grid array is indexed by row first, then column.
 */
typedef struct grid
{
    grid_cell_t *p_grid_array; ///< Pointer to the first element
                               ///< of the grid array.
    uint16_t rows;             ///< Number of rows in the grid.
    uint16_t columns;          ///< Number of columns in the grid.
} grid_t;

/**
 * @typedef navigator_state_t
 * @brief This struct contains the state of a navigator in the maze.
 *
 * @param p_current_node Pointer to the current location of the navigator.
 * @param p_start_node Pointer to the start node of the maze.
 * @param p_end_node Pointer to the end node of the maze.
 * @param orientation Orientation of the navigator. This is an enum.
 */
typedef struct navigator_state
{
    grid_cell_t *p_current_node; ///< Pointer to the current
                                 ///< location of the navigator.
    grid_cell_t *p_start_node;   ///< Pointer to the start node of the maze.
    grid_cell_t *p_end_node;     ///< Pointer to the end node of the maze.
    cardinal_direction_t orientation; ///< Orientation of the navigator.
                                      ///< This is an enum.
} navigator_state_t;

/**
 * @typedef maze_gap_bitmask_t
 * @brief This struct contains the maze gap bitmasks for serialisation.
 *
 * @param p_bitmask Pointer to the bitmask array.
 * @param rows Number of rows.
 * @param columns Number of columns.
 */
typedef struct maze_gap_bitmask
{
    uint16_t *p_bitmask; ///< Pointer to the bitmask array. This is indexed by
                         ///< row first, then column.
    uint16_t rows;       ///< Number of rows.
    uint16_t columns;    ///< Number of columns.
} maze_gap_bitmask_t;

// Public Functions.
//
grid_t               create_maze(uint16_t rows, uint16_t columns);
void                 initialise_empty_maze_walled(grid_t *p_grid);
void                 clear_maze_heuristics(grid_t *p_grid);
void                 destroy_maze(grid_t *p_grid);
void                 navigator_unset_walls(grid_t            *p_grid,
                                           navigator_state_t *p_navigator,
                                           uint8_t            wall_bitmask);
void                 navigator_modify_walls(grid_t            *p_grid,
                                            navigator_state_t *p_navigator,
                                            uint8_t            aligned_wall_bitmask,
                                            bool               is_set,
                                            bool               is_unset);
char                *get_maze_string(grid_t *p_grid);
cardinal_direction_t get_direction_from_to(point_t *p_point_a,
                                           point_t *p_point_b);

int16_t deserialise_maze(grid_t *p_grid, maze_gap_bitmask_t *p_no_walls_array);
maze_gap_bitmask_t serialise_maze(grid_t *p_grid);
grid_cell_t *get_cell_at_coordinates(grid_t *p_grid, point_t *p_coordinates);
grid_cell_t *get_cell_in_direction(grid_t              *p_grid,
                                   grid_cell_t         *p_from,
                                   cardinal_direction_t direction);
int8_t       get_offset_from_nav_direction(navigator_state_t *p_navigator);

uint32_t manhattan_distance(point_t *point_a, point_t *point_b);
void     insert_navigator_str(grid_t            *p_grid,
                              navigator_state_t *p_navigator,
                              char              *maze_str);

#endif // MAZE_H

// End of pathfinding/maze.h
