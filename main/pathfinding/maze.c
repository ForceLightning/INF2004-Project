/**
 * @file maze.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Contains the implementation of utility maze functions.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "maze.h"

// Private function prototypes.
//
static grid_cell_t *get_cell_in_direction_from(grid_t              *p_grid,
                                               grid_cell_t         *p_current,
                                               cardinal_direction_t direction);

static void set_wall_helper(grid_t      *p_grid,
                            grid_cell_t *p_current_node,
                            uint8_t      cardinal_direction);

static void unset_wall_helper(grid_t      *p_grid,
                              grid_cell_t *p_current_node,
                              uint8_t      cardinal_direction);

static void draw_cell(const grid_cell_t *p_cell,
                      char              *p_maze_string,
                      uint16_t           relative_row);

/**
 * @brief Create a maze with the specified number of rows and columns.
 *
 * @param[in] rows Number of rows in the maze.
 * @param[in] columns Number of columns in the maze.
 * @return grid_t Empty maze. Indexed first by row, then column.
 *
 * @warning The maze must be destroyed by @ref destroy_maze.
 */
grid_t
create_maze (uint16_t rows, uint16_t columns)
{
    grid_cell_t *p_grid_array = malloc(sizeof(grid_cell_t) * rows * columns);
    memset(p_grid_array, 0, sizeof(grid_cell_t) * rows * columns);
    grid_t grid = { p_grid_array, rows, columns };
    initialise_empty_maze_walled(&grid);
    return grid;
}

/**
 * @brief Initialises an empty maze. This sets the coordinates of each node, all
 * heuristic values to 0, and all pointers to NULL. This is useful for the A*
 * algorithm.
 *
 * @param[in,out] p_grid Pointer to an empty maze created by @ref create_maze.
 */
void
initialise_empty_maze_walled (grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->coordinates = (point_t) { col, row };
            p_cell->f           = 0;
            p_cell->g           = 0;
            p_cell->h           = 0;
            p_cell->p_came_from = NULL;
            p_cell->is_visited  = false;
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                p_cell->p_next[direction] = NULL;
            }
        }
    }
}

/**
 * @brief Clears the maze heuristics. This sets the F, G, and H values of each
 * node to UINT32_MAX for the A* and floodfill algorithm.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
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
            p_cell->f          = UINT32_MAX;
            p_cell->g          = UINT32_MAX;
            p_cell->h          = UINT32_MAX;
            p_cell->is_visited = false;
        }
    }
}

/**
 * @brief Destroys the maze by freeing the memory allocated to the grid array.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
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
 * possible direction.
 * @par This is positive for clockwise turns and negative for
 * anticlockwise turns. It is the minimum number of turns required to get from
 * the current direction to the desired direction.
 *
 * @param[in] p_navigator Pointer to the navigator.
 * @return int8_t Offset of the current direction from cardinal directions.
 */
int8_t
get_offset_from_nav_direction (const navigator_state_t *p_navigator)
{
    int8_t offset = (int8_t)(-p_navigator->orientation);
    return offset;
}

/**
 * @brief Unsets the walls of the current node and the next node(s) from the
 * position of the navigator in the directions it detects can be moved to.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_navigator Pointer to the navigator.
 * @param[in] wall_bitmask Bitmask of the walls to unset. This is aligned to the
 * navigator's orientation.
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

/**
 * @brief Modifies the walls of the navigator's current node.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_navigator Pointer to the navigator.
 * @param[in] aligned_wall_bitmask Bitmask of the walls to set or unset. This is
 * aligned to NORTH.
 * @param[in] is_set True if the walls are to be set.
 * @param[in] is_unset True if the walls are to be unset.
 *
 * @note both is_set and is_unset can be True. In this case, the
 * entire cell's walls and gaps will be set.
 */
void
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
        if ((is_set ^ is_unset) && (aligned_wall_bitmask & (1 << (direction))))
        {
            is_set ? set_wall_helper(
                p_grid, p_navigator->p_current_node, direction)
                   : unset_wall_helper(
                       p_grid, p_navigator->p_current_node, direction);
        }
        else if (is_set && is_unset)
        {
            // Unsets the wall in the direction where the bitmask is 1, and sets
            // the wall where the bitmask is 0.
            (aligned_wall_bitmask & (1 << direction))
                ? unset_wall_helper(
                    p_grid, p_navigator->p_current_node, direction)
                : set_wall_helper(
                    p_grid, p_navigator->p_current_node, direction);
        }
    }
}

/**
 * @brief Get the string representation of the maze for pretty printing.
 * @param[in] p_grid Pointer to the maze grid.
 * @return char* Pointer to the string representation of the maze.
 *
 * @warning The string returned by this function must be freed.
 *
 */
char *
get_maze_string (grid_t *p_grid)
{
    uint16_t maze_string_length
        = (p_grid->columns * 4 + 2) * (p_grid->rows * 2 + 1);
    char *p_maze_string = malloc(sizeof(char) * maze_string_length);
    memset(p_maze_string, 0, sizeof(char) * maze_string_length);

    // Draw the maze. For each row draw the west wall, north wall, and the path.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t relative_row = 0; relative_row < 3; relative_row++)
        {
            for (uint16_t col = 0; p_grid->columns > col; col++)
            {
                const grid_cell_t *p_cell
                    = &p_grid->p_grid_array[row * p_grid->columns + col];

                draw_cell(p_cell, p_maze_string, relative_row);
                if (p_grid->columns - 1 == col)
                {
                    switch (relative_row)
                    {
                        case 0:
                            strcat(p_maze_string, "+\n");
                            break;
                        case 1:
                            strcat(p_maze_string, "|\n");
                            break;
                        default:
                            break;
                    }
                }
            }
            // Draw the east wall of the maze. This is done outside of the loop
            // for the columns.
            //
        }
    }
    // Draw the bottom border and east wall.
    //
    for (uint16_t col = 0; p_grid->columns > col; col++)
    {
        strcat(p_maze_string, "+---");
    }
    strcat(p_maze_string, "+");

    return p_maze_string;
}

/**
 * @brief Inserts the navigator character into the maze string for pretty
 * printing.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @param[in] p_navigator Pointer to the navigator.
 * @param[in,out] p_maze_str Pointer to the maze string returned by @ref
 * get_maze_string.
 */
void
insert_navigator_str (const grid_t            *p_grid,
                      const navigator_state_t *p_navigator,
                      char                    *p_maze_str)
{
    uint16_t row = p_navigator->p_current_node->coordinates.y;
    uint16_t col = p_navigator->p_current_node->coordinates.x;

    char navigator_char = 'X';

    switch (p_navigator->orientation)
    {
        case NORTH:
            navigator_char = '^';
            break;
        case EAST:
            navigator_char = '>';
            break;
        case SOUTH:
            navigator_char = 'v';
            break;
        case WEST:
            navigator_char = '<';
            break;
        default:
            break;
    }
    uint16_t str_row      = row * 2 + 1;
    uint16_t str_col      = col * 4 + 2;
    uint16_t str_num_cols = p_grid->columns * 4 + 2;

    p_maze_str[str_row * str_num_cols + str_col] = navigator_char;
}

/**
 * @brief Get the direction from p_point_a to p_point_b if they
 * are adjacent.
 *
 * @param[in] p_point_a Pointer to point a.
 * @param[in] p_point_b Pointer to point b.
 * @return cardinal_direction_t Direction from point a to point b. NONE if they
 * are not adjacent.
 */
cardinal_direction_t
get_direction_from_to (const point_t *p_point_a, const point_t *p_point_b)
{
    int32_t row_offset = p_point_b->y - p_point_a->y;
    int32_t col_offset = p_point_b->x - p_point_a->x;

    cardinal_direction_t direction = NONE;

    // Check if the points are adjacent.
    if (1 != (abs(row_offset) + abs(col_offset)))
    {
        goto end;
    }
    // If so, check which direction the second point is from the first.
    else
    {
        if (1 == row_offset)
        {
            direction = SOUTH;
        }
        else if (-1 == row_offset)
        {
            direction = NORTH;
        }
        else if (1 == col_offset)
        {
            direction = EAST;
        }
        else if (-1 == col_offset)
        {
            direction = WEST;
        }
    }
end:
    return direction;
}

/**
 * @brief Deserialises the maze from a bitmask array. @ref serialise_maze.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_no_walls_array Pointer to the bitmask array of maze gaps.
 * @return int16_t 0 if successful, -1 otherwise.
 */
int16_t
deserialise_maze (grid_t *p_grid, maze_gap_bitmask_t *p_no_walls_array)
{
    int16_t ret_val = 0;
    // Ensure that the no walls array is not NULL.
    //
    if (NULL == p_no_walls_array)
    {
        ret_val = -1;
        goto end;
    }

    // Check that the dimensions are the same.
    //
    if (p_grid->rows != p_no_walls_array->rows
        || p_grid->columns != p_no_walls_array->columns)
    {
        ret_val = -1;
        goto end;
    }

    // Iterate through the array and set the walls.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            uint16_t no_walls_directions
                = p_no_walls_array->p_bitmask[row * p_grid->columns + col];

            // Set or unset the walls.
            //
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                // Check if the direction is set.
                //
                if (no_walls_directions & (1 << (direction)))
                {
                    unset_wall_helper(p_grid, p_cell, direction);
                }
                else
                {
                    set_wall_helper(p_grid, p_cell, direction);
                }
            }
        }
    }

end:
    return ret_val;
}

/**
 * @brief Serialises the maze into a bitmask array. @ref deserialise_maze.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @return maze_gap_bitmask_t Bitmask array of maze gaps. Take OxF - bitmask to
 * get the walls.
 */
maze_gap_bitmask_t
serialise_maze (grid_t *p_grid)
{
    maze_gap_bitmask_t no_walls_array = {
        .p_bitmask = malloc(sizeof(uint16_t) * p_grid->rows * p_grid->columns),
        .rows      = p_grid->rows,
        .columns   = p_grid->columns,
    };
    memset(no_walls_array.p_bitmask,
           0,
           sizeof(uint16_t) * p_grid->rows * p_grid->columns);

    // Iterate through the array and set the walls.
    //
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            const grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            uint8_t cardinal_direction = 0;

            // Check every direction and set the bitmask.
            //
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                // Check if the direction is set.
                //
                if (NULL != p_cell->p_next[direction])
                {
                    cardinal_direction |= (1 << (direction));
                }
            }
            no_walls_array.p_bitmask[row * p_grid->columns + col]
                = cardinal_direction;
        }
    }
    return no_walls_array;
}

/**
 * @brief Get the cell at the specified coordinates.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @param[in] p_coordinates Pointer to the coordinates.
 * @return grid_cell_t* Pointer to the cell. NULL if the cell is out of bounds.
 */
grid_cell_t *
get_cell_at_coordinates (grid_t *p_grid, const point_t *p_coordinates)
{
    grid_cell_t *p_cell = NULL;

    // Check if the coordinates are within the maze.
    //
    if (p_grid->rows <= p_coordinates->y || p_grid->columns <= p_coordinates->x)
    {
        goto end;
    }

    // Go y * columns + x cells from the start of the array.
    //
    p_cell = &p_grid->p_grid_array[p_coordinates->y * p_grid->columns
                                   + p_coordinates->x];

end:
    return p_cell;
}

/**
 * @brief Get the cell in the specified direction from a specific cell.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @param[in] p_from Pointer to the cell to get the next cell from.
 * @param[in] direction Cardinal direction to get the next cell from.
 *
 * @return grid_cell_t* Pointer to the cell. NULL if the cell is out of bounds.
 */
grid_cell_t *
get_cell_in_direction (grid_t              *p_grid,
                       grid_cell_t         *p_from,
                       cardinal_direction_t direction)
{
    int32_t row_diff = direction == NORTH ? -1 : direction == SOUTH ? 1 : 0;
    int32_t col_diff = direction == EAST ? 1 : direction == WEST ? -1 : 0;

    point_t coordinates = { p_from->coordinates.x + col_diff,
                            p_from->coordinates.y + row_diff };

    grid_cell_t *p_cell = get_cell_at_coordinates(p_grid, &coordinates);
    return p_cell;
}

/**
 * @brief Calculates the manhattan distance between two points.
 *
 * @param[in] p_point_a Pointer to first point.
 * @param[in] p_point_b Pointer to second point.
 * @return uint32_t Manhattan distance.
 *
 * @par The distance between two points is the sum of the absolute differences
 * of their Cartesian coordinates: @f$d = |x_1 - x_2| + |y_1 - y_2|@f$.
 *
 * @see https://en.wikipedia.org/wiki/Taxicab_geometry
 */
uint32_t
manhattan_distance (const point_t *p_point_a, const point_t *p_point_b)
{
    uint32_t x_diff = abs(p_point_a->x - p_point_b->x);
    uint32_t y_diff = abs(p_point_a->y - p_point_b->y);
    return x_diff + y_diff;
}

// Private functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Get the cell in the cardinal direction from the current cell.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @param[in] p_current Pointer to the current cell.
 * @param[in] direction Cardinal direction to get the next cell from.
 *
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
 * @brief Sets a wall in a given direction.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_current_node Pointer to the current node.
 * @param[in] cardinal_direction Cardinal direction to set the wall.
 */
static void
set_wall_helper (grid_t      *p_grid,
                 grid_cell_t *p_current_node,
                 uint8_t      cardinal_direction)
{
    p_current_node->p_next[cardinal_direction] = NULL;
    grid_cell_t *p_next_node                   = get_cell_in_direction_from(
        p_grid, p_current_node, cardinal_direction);

    // Sanity check to ensure that the next node is not NULL.
    if (NULL != p_next_node)
    {
        p_next_node->p_next[(cardinal_direction + 2) % 4] = NULL;
    }
}

/**
 * @brief Unsets a wall in a given direction.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @param[in,out] p_current_node Pointer to the current node.
 * @param[in] cardinal_direction Cardinal direction to unset the wall.
 *
 */
static void
unset_wall_helper (grid_t      *p_grid,
                   grid_cell_t *p_current_node,
                   uint8_t      cardinal_direction)
{
    grid_cell_t *p_next_node = get_cell_in_direction_from(
        p_grid, p_current_node, cardinal_direction);

    // Sanity check to ensure that the next node is not NULL.
    if (NULL != p_next_node)
    {
        p_current_node->p_next[cardinal_direction]        = p_next_node;
        p_next_node->p_next[(cardinal_direction + 2) % 4] = p_current_node;
    }
}

/**
 * @brief Draws the north and west walls of the cell.
 *
 * @param[in] p_cell Pointer to the cell.
 * @param[in,out] p_maze_string Pointer to the maze string being drawn.
 * @param[in] relative_row Relative row of the cell to the string row.
 *
 */
static void
draw_cell (const grid_cell_t *p_cell,
           char              *p_maze_string,
           uint16_t           relative_row)
{
    switch (relative_row)
    {
        case 0:
            // Draw the North wall.
            //
            if (NULL == p_cell->p_next[NORTH])
            {
                strcat(p_maze_string, "+---");
            }
            else
            {
                strcat(p_maze_string, "+   ");
            }
            break;
        case 1:
            // Draw the West wall.
            //
            if (NULL == p_cell->p_next[WEST])
            {
                strcat(p_maze_string, "|   ");
            }
            else
            {
                strcat(p_maze_string, "    ");
            }
            break;
        default:
            break;
    }
}

// End of file pathfinding/maze.c
