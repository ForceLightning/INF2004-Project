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

static int test_manhattan_distance(void);
static int test_create_maze(void);
static int test_initialise_empty_maze(void);
static int test_clear_maze_heuristics(void);
static int test_destroy_maze(void);
static int test_row_pathfinding(void);

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
        default:
            printf("Invalid Test #%d. Terminating.\n", choice);
            ret_val = -1;
    }

    return ret_val;
}

/**
 * @brief Tests whether the manhattan distance function works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_manhattan_distance (void)
{
    point_t point_a = { 0, 0 };
    for (uint16_t row = 0; GRID_ROWS > row; row++)
    {
        for (uint16_t col = 0; GRID_COLS > col; col++)
        {
            point_t  point_b  = { row, col };
            uint32_t distance = manhattan_distance(&point_a, &point_b);
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
 * @brief Tests the create_maze function to see if it works as expected.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_create_maze (void)
{
    grid_t maze    = create_maze(GRID_ROWS, GRID_COLS);
    int    ret_val = 0;

    if (NULL == maze.p_grid_array)
    {
        printf("Maze grid array is NULL.\n");
        ret_val = -1;
    }

    destroy_maze(&maze);
    return ret_val;
}

/**
 * @brief Tests the initialise_empty_maze function to see if it works as
 * expected. This should set the coordinates appropriately, heuristics to 0 and
 * all pointers to NULL.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_initialise_empty_maze (void)
{
    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);

    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            // Check coordinates.
            //
            grid_cell_t *p_cell = &maze.p_grid_array[row * maze.columns + col];
            if (row != p_cell->coordinates.x || col != p_cell->coordinates.y)
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
    destroy_maze(&maze);
    return 0;
}

/**
 * @brief Tests the clear_maze_heuristics function to see if it works as
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
    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);

    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            grid_cell_t *p_cell = &maze.p_grid_array[row * maze.columns + col];
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
    clear_maze_heuristics(&maze);
    for (uint16_t row = 0; maze.rows > row; row++)
    {
        for (uint16_t col = 0; maze.columns > col; col++)
        {
            grid_cell_t *p_cell = &maze.p_grid_array[row * maze.columns + col];

            if (0 != p_cell->f || 0 != p_cell->g || 0 != p_cell->h)
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
 * @brief Tests the destroy_maze function to see if it works as expected. This
 * should free the memory allocated to the grid array.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_destroy_maze (void)
{
    // Create a maze.
    //
    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);
    destroy_maze(&maze);

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
test_coords_iseq (point_t *p_point_a, point_t *p_point_b)
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
    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);

    navigator_state_t navigator_state = {
        &maze.p_grid_array[0],
        &maze.p_grid_array[0],
        // Conversion to ptrdiff_t is ok because the result is always positive.
        &maze.p_grid_array[(ptrdiff_t)(GRID_ROWS * (GRID_COLS - 1))],
        NORTH
    };

    // Set the walls of the maze. Should just be a column that leads to the
    // objective.
    //
    grid_cell_t *p_current_node = navigator_state.p_current_node;

    for (uint16_t row = 0; GRID_ROWS - 1 > row; row++)
    {
        // Conversion to ptrdiff_t is ok because the result is always positive.
        p_current_node->p_next[NORTH]
            = &maze.p_grid_array[(ptrdiff_t)((row + 1) * GRID_COLS)];
        p_current_node->p_next[NORTH]->p_next[SOUTH] = p_current_node;
        p_current_node = p_current_node->p_next[NORTH];
    }

    // Run the A* algorithm.
    //
    a_star(maze, navigator_state.p_start_node, navigator_state.p_end_node);
    grid_cell_t *p_path = get_path(navigator_state.p_end_node);

    int ret_val = 0;
    // Check that the path is correct.
    //
    if (NULL == p_path)
    {
        printf("Path is NULL.\n");
        ret_val = -1;
        goto end;
    }

    for (uint16_t row = 0; GRID_ROWS > row; row++)
    {
        point_t *point_a = &p_path[row].coordinates;
        point_t *point_b = &maze.p_grid_array[row * GRID_COLS].coordinates;

        if (!test_coords_iseq(point_a, point_b))
        {
            printf("Path is incorrect at row, col (%d, %d)\n", row, 0);
            ret_val = -1;
            goto end;
        }
    }

end: // Clean up all malloc'd memory.
    p_current_node                 = NULL;
    navigator_state.p_current_node = NULL;
    navigator_state.p_end_node     = NULL;
    navigator_state.p_start_node   = NULL;

    if (NULL != p_path)
    {
        free(p_path);
        p_path = NULL;
    }

    destroy_maze(&maze);

    return ret_val;
}

// End of file tests/tests.c
