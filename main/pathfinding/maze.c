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
static maze_grid_cell_t *maze_get_cell_in_dir_from(
    maze_grid_t              *p_grid,
    maze_grid_cell_t         *p_current,
    maze_cardinal_direction_t direction);

static void set_wall_helper(maze_grid_t      *p_grid,
                            maze_grid_cell_t *p_current_node,
                            uint8_t           cardinal_direction);

static void unset_wall_helper(maze_grid_t      *p_grid,
                              maze_grid_cell_t *p_current_node,
                              uint8_t           cardinal_direction);

static void draw_cell(const maze_grid_cell_t *p_cell,
                      char                   *p_maze_string,
                      uint16_t                relative_row);

static maze_bitmask_compressed_t *serialised_to_compressed(
    const maze_gap_bitmask_t *p_bitmask);

/**
 * @brief Create a maze with the specified number of rows and columns.
 *
 * @param[in] rows Number of rows in the maze.
 * @param[in] columns Number of columns in the maze.
 * @return maze_grid_t Empty maze. Indexed first by row, then column.
 *
 * @warning The maze must be destroyed by @ref destroy_maze.
 */
maze_grid_t
maze_create (uint16_t rows, uint16_t columns)
{
    maze_grid_cell_t *p_grid_array
        = malloc(sizeof(maze_grid_cell_t) * rows * columns);
    memset(p_grid_array, 0, sizeof(maze_grid_cell_t) * rows * columns);
    maze_grid_t grid = { p_grid_array, rows, columns };
    maze_initialise_empty_walled(&grid);
    return grid;
}

/**
 * @brief Initialises an empty maze. This sets the coordinates of each node, all
 * heuristic values to 0, and all pointers to NULL. This is useful for the A*
 * algorithm.
 *
 * @param[in,out] p_grid Pointer to an empty maze created by @ref maze_create.
 */
void
maze_initialise_empty_walled (maze_grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            p_cell->coordinates = (maze_point_t) { col, row };
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
maze_clear_heuristics (maze_grid_t *p_grid)
{
    for (uint16_t row = 0; p_grid->rows > row; row++)
    {
        for (uint16_t col = 0; p_grid->columns > col; col++)
        {
            maze_grid_cell_t *p_cell
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
maze_destroy (maze_grid_t *p_grid)
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
maze_get_nav_dir_offset (const maze_navigator_state_t *p_navigator)
{
    int8_t offset = (int8_t)(-p_navigator->orientation);
    return offset;
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
maze_nav_modify_walls (maze_grid_t            *p_grid,
                       maze_navigator_state_t *p_navigator,
                       uint8_t                 aligned_wall_bitmask,
                       bool                    is_set,
                       bool                    is_unset)
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
maze_get_string (maze_grid_t *p_grid)
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
                const maze_grid_cell_t *p_cell
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
 * maze_get_string.
 */
void
maze_insert_nav_str (const maze_grid_t            *p_grid,
                     const maze_navigator_state_t *p_navigator,
                     char                         *p_maze_str)
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
 * @return maze_cardinal_direction_t Direction from point a to point b. NONE if
 * they are not adjacent.
 */
maze_cardinal_direction_t
maze_get_dir_from_to (const maze_point_t *p_point_a,
                      const maze_point_t *p_point_b)
{
    int32_t row_offset = p_point_b->y - p_point_a->y;
    int32_t col_offset = p_point_b->x - p_point_a->x;

    maze_cardinal_direction_t direction = NONE;

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
 * @brief Deserialises the maze from a bitmask array. @ref maze_serialise.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_no_walls_array Pointer to the bitmask array of maze gaps.
 * @return int16_t 0 if successful, -1 otherwise.
 */
int16_t
maze_deserialise (maze_grid_t *p_grid, maze_gap_bitmask_t *p_no_walls_array)
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
            maze_grid_cell_t *p_cell
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
 * @brief Serialises the maze into a bitmask array. @ref maze_deserialise.
 *
 * @param[in] p_grid Pointer to the maze grid.
 * @return maze_gap_bitmask_t Bitmask array of maze gaps. Take OxF - bitmask to
 * get the walls.
 */
maze_gap_bitmask_t
maze_serialise (maze_grid_t *p_grid)
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
            const maze_grid_cell_t *p_cell
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
 * @return maze_grid_cell_t* Pointer to the cell. NULL if the cell is out of
 * bounds.
 */
maze_grid_cell_t *
maze_get_cell_at_coords (maze_grid_t *p_grid, const maze_point_t *p_coordinates)
{
    maze_grid_cell_t *p_cell = NULL;

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
 * @return maze_grid_cell_t* Pointer to the cell. NULL if the cell is out of
 * bounds.
 */
maze_grid_cell_t *
maze_get_cell_in_dir (maze_grid_t              *p_grid,
                      maze_grid_cell_t         *p_from,
                      maze_cardinal_direction_t direction)
{
    int32_t row_diff = direction == NORTH ? -1 : direction == SOUTH ? 1 : 0;
    int32_t col_diff = direction == EAST ? 1 : direction == WEST ? -1 : 0;

    maze_point_t coordinates = { p_from->coordinates.x + col_diff,
                                 p_from->coordinates.y + row_diff };

    maze_grid_cell_t *p_cell = maze_get_cell_at_coords(p_grid, &coordinates);
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
maze_manhattan_dist (const maze_point_t *p_point_a,
                     const maze_point_t *p_point_b)
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
 * @return maze_grid_cell_t* Pointer to the cell in the direction from the
 * current cell.
 */
static maze_grid_cell_t *
maze_get_cell_in_dir_from (maze_grid_t              *p_grid,
                           maze_grid_cell_t         *p_current,
                           maze_cardinal_direction_t direction)
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
 * @brief Serialises the maze into a uint8_t buffer.
 *
 * @param p_bitmask Pointer to the maze gap bitmask.
 * @param p_buffer Pointer to the buffer.
 * @param buffer_size Size of the buffer.
 * @return int16_t -1 if the buffer is too small, 0 otherwise.
 *
 * @note The buffer must be large enough to fit the maze, and the buffer must be
 * cleared before calling this function.
 */
int16_t
maze_serialised_to_buffer (const maze_gap_bitmask_t *p_bitmask,
                           uint8_t                  *p_buffer,
                           uint16_t                  buffer_size)
{
    // Check that the buffer is large enough.
    //
    uint16_t num_compressed = (p_bitmask->rows * p_bitmask->columns) / 2
                              + (p_bitmask->rows * p_bitmask->columns) % 2;

    uint16_t header_size = 4; // 2 x uint8_t for rows and columns.

    if (buffer_size < num_compressed + header_size)
    {
        return -1;
    }

    // Otherwise, iterate through the bitmask and compress it.
    //
    maze_bitmask_compressed_t *p_compressed
        = serialised_to_compressed(p_bitmask);

    // Write the header.
    //
    p_buffer[0] = (p_bitmask->rows & 0xFF00) >> 8;    // MSB
    p_buffer[1] = (p_bitmask->rows & 0x00FF);         // LSB
    p_buffer[2] = (p_bitmask->columns & 0xFF00) >> 8; // MSB
    p_buffer[3] = (p_bitmask->columns & 0x00FF);      // LSB

    // Write the compressed bitmask.
    //
    for (size_t idx = 0; num_compressed > idx; idx++)
    {
        p_buffer[idx + header_size] = p_compressed[idx].bits;
    }

    free(p_compressed); // Free the compressed array. Prevents memory leak.

    return 0; // Success.
}

/**
 * @brief Serialises the navigator state into a uint8_t buffer.
 *
 * @param p_navigator Pointer to the navigator state.
 * @param p_buffer Pointer to the buffer.
 * @param buffer_size Length of the buffer for checks.
 * @return int16_t -1 if the buffer is too small, 0 otherwise.
 *
 * @note The buffer is expected to be cleared before calling this function.
 *
 * @warning The buffer must be large enough to fit the navigator state.
 */
int16_t
maze_nav_to_buffer (const maze_navigator_state_t *p_navigator,
                    uint8_t                      *p_buffer,
                    uint16_t                      buffer_size)
{
    // Puts the navigator's coordinates, orientation, current node, start node,
    // end node coordinates in that order.
    //
    const uint16_t header_size
        = 13u; // 2 x uint16_t for coordinates, 1 x uint8_t for orientation, 4 x
               // uint16_t for start and end node coordinates. Value in bytes.

    if (header_size > buffer_size)
    {
        return -1;
    }

    maze_uint16_to_uint8_buffer(p_navigator->p_current_node->coordinates.x,
                                &p_buffer[0]);
    maze_uint16_to_uint8_buffer(p_navigator->p_current_node->coordinates.y,
                                &p_buffer[2]);
    p_buffer[4] = (uint8_t)p_navigator->orientation;
    maze_uint16_to_uint8_buffer(p_navigator->p_start_node->coordinates.x,
                                &p_buffer[5]);
    maze_uint16_to_uint8_buffer(p_navigator->p_start_node->coordinates.y,
                                &p_buffer[7]);
    maze_uint16_to_uint8_buffer(p_navigator->p_end_node->coordinates.x,
                                &p_buffer[9]);
    maze_uint16_to_uint8_buffer(p_navigator->orientation, &p_buffer[11]);

    return 0;
}

/**
 * @brief Helper function to convert a uint16_t to a uint8_t buffer.
 *
 * @param value Value to convert.
 * @param p_buffer uint8_t buffer of size 2.
 */
void
maze_uint16_to_uint8_buffer (uint16_t value, uint8_t *p_buffer)
{
    p_buffer[0] = (value & 0xFF00) >> 8;
    p_buffer[1] = (value & 0x00FF);
}

// Private functions definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Sets a wall in a given direction.
 *
 * @param[in,out] p_grid Pointer to the maze grid.
 * @param[in] p_current_node Pointer to the current node.
 * @param[in] cardinal_direction Cardinal direction to set the wall.
 */
static void
set_wall_helper (maze_grid_t      *p_grid,
                 maze_grid_cell_t *p_current_node,
                 uint8_t           cardinal_direction)
{
    p_current_node->p_next[cardinal_direction] = NULL;
    maze_grid_cell_t *p_next_node
        = maze_get_cell_in_dir_from(p_grid, p_current_node, cardinal_direction);

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
unset_wall_helper (maze_grid_t      *p_grid,
                   maze_grid_cell_t *p_current_node,
                   uint8_t           cardinal_direction)
{
    maze_grid_cell_t *p_next_node
        = maze_get_cell_in_dir_from(p_grid, p_current_node, cardinal_direction);

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
draw_cell (const maze_grid_cell_t *p_cell,
           char                   *p_maze_string,
           uint16_t                relative_row)
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

static maze_bitmask_compressed_t *
serialised_to_compressed (const maze_gap_bitmask_t *p_bitmask)
{
    uint8_t                    num_cells = p_bitmask->rows * p_bitmask->columns;
    maze_bitmask_compressed_t *p_compressed
        = malloc(sizeof(maze_bitmask_compressed_t) * num_cells);
    memset(p_compressed, 0, sizeof(maze_bitmask_compressed_t) * num_cells);

    uint16_t num_compressed = num_cells / 2 + num_cells % 2;

    for (size_t cell = 0; num_compressed > cell; cell++)
    {
        p_compressed[cell].fields.cell_a = p_bitmask->p_bitmask[cell * 2];

        if (num_cells > cell * 2 + 1)
        {
            p_compressed[cell].fields.cell_b
                = p_bitmask->p_bitmask[cell * 2 + 1];
        }
    }

    return p_compressed;
}

// End of file pathfinding/maze.c
