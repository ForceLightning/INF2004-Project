#include <stdio.h>
#include <stdint.h>
#include "pathfinding/a_star.h"
#include "pathfinding/maze.h"

static int test_manhattan_distance(void);
static int test_create_maze(void);
static int test_initialise_empty_maze(void);
static int test_clear_maze_heuristics(void);
static int test_destroy_maze(void);
static int test_column_pathfinding(void);

int
pathfinding_tests (int argc, char *argv[])
{
    int default_choice = 1;
    int choice         = default_choice;

    if (1 < argc)
    {
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
            ret_val = test_column_pathfinding();
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
    for (uint16_t row = 0; 10 > row; row++)
    {
        for (uint16_t col = 0; 10 > col; col++)
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
    // TODO(chris): Implement this.
    return 0;
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
    // TODO(chris): Implement this.
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
    // TODO(chris): Implement this.
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
    // TODO(chris): Implement this.

    // Step 1: Create a maze.
    return 0;
}

/**
 * @brief Tests the column pathfinding function to see if it works as expected.
 * This should return an array of nodes in sequential order that represent the
 * shortest path from the start node to the end node.
 *
 * @return int
 */
static int
test_column_pathfinding (void)
{
    // TODO(chris): Implement this.
    return 0;
}

// End of file tests/tests.c
