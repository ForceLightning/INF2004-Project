#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maze.h"

// Private function prototypes.
//
static int8_t get_offset_from_nav_direction(navigator_state_t *p_navigator);

static grid_cell_t *get_cell_in_direction_from(grid_t              *p_grid,
                                               grid_cell_t         *p_current,
                                               cardinal_direction_t direction);

static void navigator_modify_walls(grid_t            *p_grid,
                                   navigator_state_t *p_navigator,
                                   uint8_t            aligned_wall_bitmask,
                                   bool               is_set,
                                   bool               is_unset);

static void navigator_set_wall_helper(grid_t            *p_grid,
                                      navigator_state_t *p_navigator,
                                      uint8_t            cardinal_direction);

static void navigator_unset_wall_helper(grid_t            *p_grid,
                                        navigator_state_t *p_navigator,
                                        uint8_t            cardinal_direction);

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
    if (NULL != p_grid->p_grid_array)
    {
        free(p_grid->p_grid_array);
        p_grid->p_grid_array = NULL;
    }

    p_grid->rows    = 0;
    p_grid->columns = 0;
}

/**
 * @brief Get the offset from the navigator's current direction to any other
 * possible direction. This is positive for clockwise turns and negative for
 * anticlockwise turns. It is the minimum number of turns required to get from
 * the current direction to the desired direction.
 *
 * @param p_navigator Pointer to the navigator.
 * @return int8_t Offset of the current direction from cardinal directions.
 */
static int8_t
get_offset_from_nav_direction (navigator_state_t *p_navigator)
{
    int8_t offset = (int8_t)(-p_navigator->orientation);
    return offset;
}

/**
 * @brief Get the cell in the cardinal direction from the current cell.
 *
 * @param p_grid Pointer to the maze.
 * @param p_current Pointer to the current cell.
 * @param direction Cardinal direction to get the cell from.
 * @return grid_cell_t* Pointer to the cell in the direction from the current
 * cell.
 */
static grid_cell_t *
get_cell_in_direction_from (grid_t              *p_grid,
                            grid_cell_t         *p_current,
                            cardinal_direction_t direction)
{
    int8_t row_offset = 0;
    int8_t col_offset = 0;

    switch (direction)
    {
        case NORTH:
            row_offset = -1;
            break;
        case EAST:
            col_offset = 1;
            break;
        case SOUTH:
            row_offset = 1;
            break;
        case WEST:
            col_offset = -1;
            break;
        default:
            break;
    }

    uint16_t row = p_current->coordinates.y + row_offset;
    uint16_t col = p_current->coordinates.x + col_offset;

    if (p_grid->rows <= row || p_grid->columns <= col)
    {
        return NULL;
    }

    return &p_grid->p_grid_array[row * p_grid->columns + col];
}

/**
 * @brief Unsets the walls of the current node and the next node(s) from the
 * position of the navigator in the directions it detects can be moved to.
 *
 * @param p_grid Pointer to the maze.
 * @param p_navigator Pointer to the navigator.
 * @param wall_bitmask Bitmask of the walls to unset.
 */
void
navigator_unset_walls (grid_t            *p_grid,
                       navigator_state_t *p_navigator,
                       uint8_t            wall_bitmask)
{
    int8_t  offset     = get_offset_from_nav_direction(p_navigator);
    uint8_t mod_offset = offset % 4;
    uint8_t wall_bitmask_rotated
        = (wall_bitmask << mod_offset) | (wall_bitmask >> (4 - mod_offset));

    if (0 == offset)
    {
        return;
    }

    // Set the next pointers of the current node.
    //
    navigator_modify_walls(
        p_grid, p_navigator, wall_bitmask_rotated, true, false);
}

static void
navigator_modify_walls (grid_t            *p_grid,
                        navigator_state_t *p_navigator,
                        uint8_t            aligned_wall_bitmask,
                        bool               is_set,
                        bool               is_unset)
{
    // Sanity check to ensure that the navigator is not trying to do nothing
    // with this function call.
    if (!is_set && !is_unset)
    {
        return;
    }

    for (int8_t direction = 0; 4 > direction; direction++)
    {
        // Check if only one operation is being performed.
        if ((is_set ^ is_unset) && (aligned_wall_bitmask & (1 << direction)))
        {
            is_set
                ? navigator_set_wall_helper(p_grid, p_navigator, direction)
                : navigator_unset_wall_helper(p_grid, p_navigator, direction);
        }
        else
        {
            // Unsets the wall in the direction where the bitmask is 1, and sets
            // the wall where the bitmask is 0.
            aligned_wall_bitmask & (1 << direction)
                ? navigator_unset_wall_helper(p_grid, p_navigator, direction)
                : navigator_set_wall_helper(p_grid, p_navigator, direction);
        }
    }
}

/**
 * @brief Sets a wall in a given direction.
 *
 * @param p_grid Pointer to the maze grid.
 * @param p_navigator Pointer to the navigator.
 * @param cardinal_direction Cardinal direction to set the wall in.
 */
static void
navigator_set_wall_helper (grid_t            *p_grid,
                           navigator_state_t *p_navigator,
                           uint8_t            cardinal_direction)
{
    p_navigator->p_current_node->p_next[cardinal_direction] = NULL;
    grid_cell_t *p_next_node = get_cell_in_direction_from(
        p_grid, p_navigator->p_current_node, cardinal_direction);

    // Sanity check to ensure that the next node is not NULL.
    if (NULL != p_next_node)
    {
        p_next_node->p_next[(cardinal_direction + 2) % 4] = NULL;
    }
}

/**
 * @brief Unsets a wall in a given direction.
 *
 * @param p_grid Pointer to the maze grid.
 * @param p_navigator Pointer to the navigator.
 * @param cardinal_direction Cardinal direction to unset the wall in.
 */
static void
navigator_unset_wall_helper (grid_t            *p_grid,
                             navigator_state_t *p_navigator,
                             uint8_t            cardinal_direction)
{
    grid_cell_t *p_next_node = get_cell_in_direction_from(
        p_grid, p_navigator->p_current_node, cardinal_direction);

    // Sanity check to ensure that the next node is not NULL.
    if (NULL != p_next_node)
    {
        p_navigator->p_current_node->p_next[cardinal_direction] = p_next_node;
        p_next_node->p_next[(cardinal_direction + 2) % 4]
            = p_navigator->p_current_node;
    }
}