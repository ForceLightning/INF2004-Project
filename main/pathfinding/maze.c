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

static void draw_cell(grid_cell_t *p_cell,
                      char        *p_maze_string,
                      uint16_t     relative_row);

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
    initialise_empty_maze_walled(&grid);
    return grid;
}

/**
 * @brief Initialises an empty maze. This sets the coordinates of each node, all
 * heuristic values to 0, and all pointers to NULL.
 *
 * @param grid Pointer to empty maze.
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
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                p_cell->p_next[direction] = NULL;
            }
        }
    }
}

/**
 * @brief Clears the maze heuristics. This sets the F, G, and H values of each
 * node to UINT32_MAX for the A* algorithm.
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
            p_cell->f = UINT32_MAX;
            p_cell->g = UINT32_MAX;
            p_cell->h = UINT32_MAX;
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
int8_t
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
            aligned_wall_bitmask & (1 << direction)
                ? unset_wall_helper(
                    p_grid, p_navigator->p_current_node, direction)
                : set_wall_helper(
                    p_grid, p_navigator->p_current_node, direction);
        }
    }
}

/**
 * @brief Sets a wall in a given direction.
 *
 * @param p_grid Pointer to the maze grid.
 * @param p_current_node Pointer to the current node.
 * @param cardinal_direction Cardinal direction to set the wall in.
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
 * @param p_grid Pointer to the maze grid.
 * @param p_current_node Pointer to the navigator.
 * @param cardinal_direction Cardinal direction to unset the wall in.
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
 * @brief Get the string representation of the maze.
 *
 * @param p_grid Pointer to the maze.
 * @return char* Pointer to string representation of the maze.
 *
 * @par This returns an ASCII string representation of the maze. Maze walls are
 * represented with --- or | depending on the direction of the wall.
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
                grid_cell_t *p_cell
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
 * @brief Inserts the navigator into the maze string.
 *
 * @param p_grid Pointer to the maze.
 * @param p_navigator Pointer to the navigator state.
 * @param maze_str Pointer to the maze string created by @ref get_maze_string.
 */
void
insert_navigator_str (grid_t            *p_grid,
                      navigator_state_t *p_navigator,
                      char              *maze_str)
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

    maze_str[str_row * str_num_cols + str_col] = navigator_char;
}

/**
 * @brief Draws the north and west walls of the cell.
 *
 * @param p_cell Pointer to the cell.
 * @param p_maze_string Pointer to the maze string.
 * @param relative_row Internal row count of the string of the cell.
 */
static void
draw_cell (grid_cell_t *p_cell, char *p_maze_string, uint16_t relative_row)
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

/**
 * @brief Get the direction from point_a to point_b if they are adjacent.
 *
 * @param p_point_a Pointer to point a.
 * @param p_point_b Pointer to point b.
 * @return cardinal_direction_t Direction from point a to point b. NONE if they
 * are not adjacent.
 */
cardinal_direction_t
get_direction_from_to (point_t *p_point_a, point_t *p_point_b)
{
    int8_t row_offset = p_point_b->y - p_point_a->y;
    int8_t col_offset = p_point_b->x - p_point_a->x;

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
 * @brief Parses a bitmask array into a maze.
 *
 * @param p_grid Pointer to the maze.
 * @param p_no_walls_array Pointer to the bitmask array.
 * @return int16_t 0 if successful, -1 if not.
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
            uint8_t cardinal_direction
                = p_no_walls_array->p_bitmask[row * p_grid->columns + col];

            // Check if the cell is NULL.
            //
            if (NULL == p_cell)
            {
                ret_val = -1;
                goto end;
            }

            // Set or unset the walls.
            //
            for (uint8_t direction = 0; 4 > direction; direction++)
            {
                // Check if the direction is set.
                //
                if (cardinal_direction & (1 << (direction)))
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
 * @brief Serialises a maze into a bitmask array.
 *
 * @param p_grid Pointer to the maze.
 * @return maze_gap_bitmask_t Bitmask array.
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
            grid_cell_t *p_cell
                = &p_grid->p_grid_array[row * p_grid->columns + col];
            uint8_t cardinal_direction = 0;

            // Check if the cell is NULL.
            //
            if (NULL == p_cell)
            {
                goto end;
            }

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

end:
    return no_walls_array;
}

/**
 * @brief Get the cell at the specified coordinates.
 *
 * @param p_grid Pointer to the maze.
 * @param p_coordinates Pointer to the coordinates.
 * @return grid_cell_t* Pointer to the cell. NULL if the coordinates are out of
 * the maze.
 */
grid_cell_t *
get_cell_at_coordinates (grid_t *p_grid, point_t *p_coordinates)
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

grid_cell_t *
get_cell_in_direction (grid_t              *p_grid,
                       grid_cell_t         *p_from,
                       cardinal_direction_t direction)
{
    int16_t row_diff = direction == NORTH ? -1 : direction == SOUTH ? 1 : 0;
    int16_t col_diff = direction == EAST ? 1 : direction == WEST ? -1 : 0;

    point_t coordinates = { p_from->coordinates.x + col_diff,
                            p_from->coordinates.y + row_diff };

    grid_cell_t *p_cell = get_cell_at_coordinates(p_grid, &coordinates);
    return p_cell;
}

/**
 * @brief Calculates the manhattan distance between two points.
 *
 * @param point_a First point.
 * @param point_b Second point.
 * @return uint32_t The manhattan distance.
 *
 * @ref https://en.wikipedia.org/wiki/Taxicab_geometry
 */
uint32_t
manhattan_distance (point_t *point_a, point_t *point_b)
{
    uint32_t x_diff = abs(point_a->x - point_b->x);
    uint32_t y_diff = abs(point_a->y - point_b->y);
    return x_diff + y_diff;
}

// End of file pathfinding/maze.c
