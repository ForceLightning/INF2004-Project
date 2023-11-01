#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "maze.h"

/**
 * @brief Create a maze with the specified number of rows and columns.
 *
 * @param rows Number of rows.
 * @param columns Number of columns.
 * @return grid_t Empty maze.
 */
grid_t
create_maze (uint16_t rows, uint16_t columns)
{
    grid_cell_t *p_grid_array = malloc(sizeof(grid_cell_t) * rows * columns);
    memset(p_grid_array, 0, sizeof(grid_cell_t) * rows * columns);
    grid_t grid = { p_grid_array, rows, columns };
    initialise_empty_maze(&grid);
    return grid;
}

/**
 * @brief Initialises an empty maze. This sets the coordinates of each node, all
 * heuristic values to 0, and all pointers to NULL.
 *
 * @param grid Pointer to empty maze.
 */
void
initialise_empty_maze (grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->coordinates = (point_t) { row, col };
            p_cell->f           = 0;
            p_cell->g           = 0;
            p_cell->h           = 0;
            p_cell->p_parent    = NULL;
            p_cell->p_came_from = NULL;
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                p_cell->p_next[direction] = NULL;
            }
        }
    }
}

/**
 * @brief Clears the maze heuristics. This sets the F, G, and H values of each
 * node to 0 for the A* algorithm.
 *
 * @param p_grid Pointer to the maze.
 */
void
clear_maze_heuristics (grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->f = 0;
            p_cell->g = 0;
            p_cell->h = 0;
        }
    }
}

/**
 * @brief Destroys the maze by freeing the memory allocated to the grid array.
 *
 * @param p_grid Pointer to the maze.
 */
void
destroy_maze (grid_t *p_grid)
{
    free(p_grid->p_grid_array);
    p_grid->p_grid_array = NULL;
    p_grid->rows         = 0;
    p_grid->columns      = 0;
}