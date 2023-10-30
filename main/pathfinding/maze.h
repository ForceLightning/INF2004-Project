#ifndef MAZE_H
#define MAZE_H
#include <stdint.h>

// Declare constants.
//
enum
{
    ROWS    = 10, // Number of rows in the grid.
    COLUMNS = 10  // Number of columns in the grid.
};

/**
 * @brief This struct contains the coordinates of a point.
 *
 * @param x X-coordinate.
 * @param y Y-coordinate.
 */
typedef struct
{
    uint16_t x;
    uint16_t y;
} point_t;

/**
 * @brief This enum contains the possible directions.
 */
enum direction
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

/**
 * @brief This struct contains the node information.
 *
 * @param coordinates X and Y coordinates of the node.
 * @param f F-value of the node. F = G + H.
 * @param g G-value of the node. G = cost to move from the starting node to the
 * @param h H-value of the node. H = estimated cost to move from the current aka
 * heuristic.
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
    point_t           coordinates;
    uint32_t          f;
    uint32_t          g;
    uint32_t          h;
    struct grid_cell *p_parent;
    struct grid_cell *p_next[4];
    struct grid_cell *p_came_from;
} grid_cell_t;

/**
 * @brief This struct contains the maze grid information.
 *
 * @param p_grid_array Pointer to the first element of the grid array.
 * @param rows Number of rows in the grid.
 * @param columns Number of columns in the grid.
 *
 * @NOTE: The grid array is indexed by row first, then column.
 */
typedef struct
{
    grid_cell_t *p_grid_array;
    uint16_t     rows;
    uint16_t     columns;
} grid_t;

#endif

// End of pathfinding/maze.h
