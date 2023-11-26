/**
 * @file pathfinding_tests.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the tests for the A* pathfinding algorithm.
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pathfinding/a_star.h"
#include "pathfinding/maze.h"

// Definitions.
//
typedef enum constants
{
    GRID_ROWS = 10, // Number of rows in the grid.
    GRID_COLS = 10  // Number of columns in the grid.
} constants_t;

static const uint16_t g_bitmask_array[25] = {
    0x2, 0xE, 0xA, 0xC, 0x4, // Top Row
    0x6, 0xB, 0xC, 0x3, 0x9, // 2nd row
    0x3, 0x8, 0x7, 0x8, 0x4, // 3rd row
    0x4, 0x4, 0x7, 0xA, 0xD, // 4th row
    0x3, 0xB, 0x9, 0x2, 0x9  // last row
};

// Test function prototypes.
// ----------------------------------------------------------------------------
//

static int test_manhattan_distance(void);
static int test_create_maze(void);
static int test_initialise_empty_maze(void);
static int test_clear_maze_heuristics(void);
static int test_destroy_maze(void);
static int test_row_pathfinding(void);
static int test_print_maze(void);
static int test_print_route(void);
static int test_maze_deserialisation(void);
static int test_maze_serialisation(void);
static int test_complex_maze_pathfinding(void);

// Private function prototypes.
// ----------------------------------------------------------------------------
//
static maze_grid_t generate_col_maze(uint16_t rows, uint16_t cols);

/**
 * @brief The main function for the pathfinding tests.
 *
 * @param[in] argc Number of arguments.
 * @param[in] argv Vector of arguments.
 * @return int 0 if the test passes, -1 otherwise.
 */
int
pathfinding_tests (int argc, char *argv[])
{
    int default_choice = 1;
    int choice         = default_choice;

    if (1 < argc)
    {
        // Unsafe conversion to int. This is ok because the input is controlled
        // by ctest.
        if (sscanf(argv[1], "%d", &choice) != 1)
        {
            printf("Could not parse argument. Terminating.\n");
            return -1;
        }
    }

    int ret_val = 0;
    switch (choice)
    {
        case 1:
            ret_val = test_manhattan_distance();
            break;
        case 2:
            ret_val = test_create_maze();
            break;
        case 3:
            ret_val = test_initialise_empty_maze();
            break;
        case 4:
            ret_val = test_clear_maze_heuristics();
            break;
        case 5:
            ret_val = test_destroy_maze();
            break;
        case 6:
            ret_val = test_row_pathfinding();
            break;
        case 7:
            ret_val = test_print_maze();
            break;
        case 8:
            ret_val = test_print_route();
            break;
        case 9:
            ret_val = test_maze_deserialisation();
            break;
        case 10:
            ret_val = test_maze_serialisation();
            break;
        case 11:
            ret_val = test_complex_maze_pathfinding();
            break;
        default:
            printf("Invalid Test #%d. Terminating.\n", choice);
            ret_val = -1;
    }

    return ret_val;
}

// Test functions
// ----------------------------------------------------------------------------
//

/**
 * @brief Tests whether the manhattan distance function works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_manhattan_distance (void)
{
    maze_point_t point_a = { 0, 0 };
    for (int32_t row = 0; GRID_ROWS > row; row++)
    {
        for (int32_t col = 0; GRID_COLS > col; col++)
        {
            maze_point_t point_b  = { row, col };
            uint32_t     distance = maze_manhattan_dist(&point_a, &point_b);
            if (row + col != distance)
            {
                printf("Distance between (%d, %d) and (%d, %d) is %d.\n",
                       point_a.x,
                       point_a.y,
                       point_b.x,
                       point_b.y,
                       distance);
                return -1;
            }
        }
    }
    return 0;
}

/**
 * @brief Tests the maze_create function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_create_maze (void)
{
    maze_grid_t maze    = maze_create(GRID_ROWS, GRID_COLS);
    int         ret_val = 0;

    if (NULL == maze.p_grid_array)
    {
        printf("Maze grid array is NULL.\n");
        ret_val = -1;
    }

    maze_destroy(&maze);
    return ret_val;
}

/**
 * @brief Tests the maze_initialise_empty_walled function to see if it works as
 * expected. This should set the coordinates appropriately, heuristics to 0 and
 * all pointers to NULL.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_initialise_empty_maze (void)
{
    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);

    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            // Check coordinates.
            //
            maze_grid_cell_t *p_cell
                = &maze.p_grid_array[row * maze.columns + col];
            if (col != p_cell->coordinates.x || row != p_cell->coordinates.y)
            {
                printf("Coordinates of cell (%d, %d) are (%d, %d).\n",
                       row,
                       col,
                       p_cell->coordinates.x,
                       p_cell->coordinates.y);
                return -1; // Return here is easier than breaking out of nested
                           // loops.
            }

            // Check heuristics.
            //
            if (0 != p_cell->f || 0 != p_cell->g || 0 != p_cell->h)
            {
                printf("Heuristics of cell (%d, %d) are (%d, %d, %d).\n",
                       row,
                       col,
                       p_cell->f,
                       p_cell->g,
                       p_cell->h);
                return -1; // Again, easier to return here.
            }

            // Check pointers.
            //
            if (NULL != p_cell->p_came_from)
            {
                printf(
                    "Parent or came_from pointer of cell (%d, %d) is not "
                    "NULL.\n",
                    row,
                    col);
                return -1;
            }
        }
    }
    maze_destroy(&maze);
    return 0;
}

/**
 * @brief Tests the maze_clear_heuristics function to see if it works as
 * expected. This should set all heuristics to 0 without affecting the other
 * node values.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_clear_maze_heuristics (void)
{
    // Initialise maze with random heuristic values.
    //
    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);

    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &maze.p_grid_array[row * maze.columns + col];
            // Casting to uint32_t is ok because rand() returns an int (0,
            // RAND_MAX). No overflow.
            //
            p_cell->f = (uint32_t)rand();
            p_cell->g = (uint32_t)rand();
            p_cell->h = (uint32_t)rand();
        }
    }

    // Clear the heuristics and check that they are all 0.
    //
    maze_clear_heuristics(&maze);
    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &maze.p_grid_array[row * maze.columns + col];

            if (UINT32_MAX != p_cell->f || UINT32_MAX != p_cell->g
                || UINT32_MAX != p_cell->h)
            {
                printf("Heuristics of cell (%d, %d) are (%d, %d, %d).\n",
                       row,
                       col,
                       p_cell->f,
                       p_cell->g,
                       p_cell->h);
                return -1; // Return to break out of the nested loop.
            }
        }
    }

    return 0;
}

/**
 * @brief Tests the maze_destroy function to see if it works as expected. This
 * should free the memory allocated to the grid array.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_destroy_maze (void)
{
    // Create a maze.
    //
    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);
    maze_destroy(&maze);

    if (NULL != maze.p_grid_array)
    {
        printf("Maze grid array pointer is not NULL.\n");
        return -1;
    }

    return 0;
}

/**
 * @brief Tests if two points are equal.
 *
 * @param p_point_a Pointer to point a.
 * @param p_point_b Pointer to point b.
 * @return true Two points are equal.
 * @return false Two points are not equal.
 */
static bool
test_coords_iseq (maze_point_t *p_point_a, maze_point_t *p_point_b)
{
    // Compare the coordinates.
    //
    if (p_point_a->x != p_point_b->x || p_point_a->y != p_point_b->y)
    {
        printf("Coordinates are not equal.\n");
        return false;
    }

    return true;
}

/**
 * @brief Tests the column pathfinding function to see if it works as expected.
 * This should return an array of nodes in sequential order that represent the
 * shortest path from the start node to the end node.
 *
 * @return int
 */
static int
test_row_pathfinding (void)
{
    // Initialise the grid and the navigator.
    //
    maze_grid_t maze = generate_col_maze(GRID_ROWS, GRID_COLS);

    maze_navigator_state_t navigator_state = {
        &maze.p_grid_array[0],
        &maze.p_grid_array[0],
        // Conversion to ptrdiff_t is ok because the result is always positive.
        &maze.p_grid_array[(ptrdiff_t)(GRID_ROWS * (GRID_COLS - 1))],
        MAZE_NORTH
    };

    // Run the A* algorithm.
    //
    a_star(&maze, navigator_state.p_start_node, navigator_state.p_end_node);
    a_star_path_t *p_path = a_star_get_path(navigator_state.p_end_node);

    int ret_val = 0;
    // Check that the path is correct.
    //
    if (NULL == p_path)
    {
        printf("Path is NULL.\n");
        ret_val = -1;
        goto end;
    }

    for (size_t row = 0; GRID_ROWS > row; row++)
    {
        maze_point_t *point_a = &p_path->p_path[row].coordinates;
        maze_point_t *point_b = &maze.p_grid_array[row * GRID_COLS].coordinates;

        if (!test_coords_iseq(point_a, point_b))
        {
            printf("Path is incorrect at row, col (%d, %d)\n", row, 0);
            ret_val = -1;
            goto end;
        }
    }

end: // Clean up all malloc'd memory.
    navigator_state.p_current_node = NULL;
    navigator_state.p_end_node     = NULL;
    navigator_state.p_start_node   = NULL;

    if (NULL != p_path)
    {
        if (NULL != p_path->p_path)
        {
            free(p_path->p_path);
            p_path->p_path = NULL;
        }
        free(p_path);
        p_path = NULL;
    }

    maze_destroy(&maze);

    return ret_val;
}

/**
 * @brief Tests the print_maze function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_print_maze (void)
{
    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);

    char *maze_str = maze_get_string(&maze);

    printf("%s\n", maze_str);

    free(maze_str);

    return 0;
}

/**
 * @brief Tests the print_route function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_print_route (void)
{
    maze_grid_t maze = generate_col_maze(GRID_ROWS, GRID_COLS);

    maze_navigator_state_t navigator_state = {
        &maze.p_grid_array[0],
        &maze.p_grid_array[0],
        // Conversion to ptrdiff_t is ok because the result is always positive.
        &maze.p_grid_array[(ptrdiff_t)(GRID_ROWS * (GRID_COLS - 1))],
        MAZE_NORTH
    };

    // Run the A* algorithm.
    //
    a_star(&maze, navigator_state.p_start_node, navigator_state.p_end_node);
    a_star_path_t *p_path = a_star_get_path(navigator_state.p_end_node);

    char *maze_str = a_star_get_path_str(&maze, p_path);

    printf("%s\n", maze_str);

    // Clean up all malloc'd memory.
    free(maze_str);

    if (NULL != p_path)
    {
        if (NULL != p_path->p_path)
        {
            free(p_path->p_path);
            p_path->p_path = NULL;
        }
    }
    return 0;
}

/**
 * @brief Tests the maze_deserialise function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_maze_deserialisation (void)
{
    int         ret_val = 0;
    maze_grid_t maze    = maze_create(5, 5);

    maze_gap_bitmask_t gap_bitmask
        = { .p_bitmask = NULL, .rows = 5, .columns = 5 };

    gap_bitmask.p_bitmask = (uint16_t *)g_bitmask_array;

    ret_val = maze_deserialise(&maze, &gap_bitmask);

    char *maze_str = maze_get_string(&maze);

    printf("%s\n", maze_str);

    return ret_val;
}

/**
 * @brief Tests the maze_serialise function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_maze_serialisation (void)
{
    int ret_val = 0;

    maze_grid_t maze = maze_create(5, 5);

    maze_gap_bitmask_t gap_bitmask
        = { .p_bitmask = NULL, .rows = 5, .columns = 5 };

    gap_bitmask.p_bitmask = (uint16_t *)g_bitmask_array;
    maze_deserialise(&maze, &gap_bitmask);

    maze_gap_bitmask_t serialised_maze = maze_serialise(&maze);

    for (uint16_t row = 0; row < serialised_maze.rows; row++)
    {
        for (uint16_t col = 0; col < serialised_maze.columns; col++)
        {
            uint16_t *p_bitmask
                = &serialised_maze
                       .p_bitmask[row * serialised_maze.columns + col];
            uint16_t *p_actual
                = &g_bitmask_array[row * serialised_maze.columns + col];
            if (*p_actual != *p_bitmask)
            {
                printf(
                    "Bitmask at row, col (%d, %d) is %x when it should be "
                    "%x.\n",
                    row,
                    col,
                    *p_bitmask,
                    *p_actual);
                ret_val = -1;
                goto end;
            }
        }
    }

end:; // Clean up all malloc'd memory, semicolon is for linting errors.
    char *maze_str = maze_get_string(&maze);
    printf("%s\n", maze_str);
    free(maze_str);

    free(serialised_maze.p_bitmask);
    serialised_maze.p_bitmask = NULL;

    return ret_val;
}

/**
 * @brief Tests the pathfinding algorithm on a complex maze.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_complex_maze_pathfinding (void)
{
    int ret_val = 0;

    maze_grid_t maze = maze_create(5, 5);

    maze_gap_bitmask_t gap_bitmask
        = { .p_bitmask = NULL, .rows = 5, .columns = 5 };

    gap_bitmask.p_bitmask = (uint16_t *)g_bitmask_array;
    maze_deserialise(&maze, &gap_bitmask);

    maze_point_t start_point = (maze_point_t) { 0, 4 };
    maze_point_t end_point   = (maze_point_t) { 4, 0 };

    maze_grid_cell_t *p_start = maze_get_cell_at_coords(&maze, &start_point);
    maze_grid_cell_t *p_end   = maze_get_cell_at_coords(&maze, &end_point);

    maze_navigator_state_t navigator_state = { p_start, p_start, p_end, MAZE_NORTH };

    // Run the A* algorithm.
    //
    a_star(&maze, navigator_state.p_start_node, navigator_state.p_end_node);
    a_star_path_t *p_path = a_star_get_path(navigator_state.p_end_node);

    if (NULL == p_path)
    {
        printf("Path is NULL.\n");
        ret_val = -1;
        goto end;
    }

    char *maze_str = a_star_get_path_str(&maze, p_path);
    printf("%s\n", maze_str);

end: // Clean up all malloc'd memory.
    navigator_state.p_current_node = NULL;
    navigator_state.p_end_node     = NULL;
    navigator_state.p_start_node   = NULL;
    free(maze_str);

    if (NULL != p_path)
    {
        if (NULL != p_path->p_path)
        {
            free(p_path->p_path);
            p_path->p_path = NULL;
        }
        free(p_path);
        p_path = NULL;
    }

    maze_destroy(&maze);

    return ret_val;
}

// Private functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Generates a maze that is a single column that leads to the objective.
 *
 * @param[in] rows Number of rows in the maze.
 * @param[in] cols Number of columns in the maze.
 * @return maze_grid_t Generated maze.
 */
static maze_grid_t
generate_col_maze (uint16_t rows, uint16_t cols)
{
    maze_grid_t maze = maze_create(rows, cols);

    // Set the walls of the maze. Should just be a column that leads to the
    // objective.
    //
    maze_grid_cell_t *p_current_node = &maze.p_grid_array[0];

    // Deal with the first node.
    //
    for (uint32_t row = 0; GRID_ROWS - 1 > row; row++)
    {
        // Conversion to ptrdiff_t is ok because the result is always positive.
        p_current_node->p_next[MAZE_SOUTH]
            = &maze.p_grid_array[(ptrdiff_t)((row + 1) * GRID_COLS)];
        p_current_node->p_next[MAZE_SOUTH]->p_next[MAZE_NORTH] = p_current_node;
        p_current_node = p_current_node->p_next[MAZE_SOUTH];
    }

    return maze;
}

// End of file tests/tests.c
