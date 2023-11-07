/**
 * @file floodfill_tests.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the tests for the floodfill algorithm.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pathfinding/floodfill.h"
#include "pathfinding/maze.h"

// Definitions.
//
#ifndef NDEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

typedef enum constants
{
    GRID_ROWS = 5, // Number of rows in the grid.
    GRID_COLS = 5  // Number of columns in the grid.
} constants_t;

/**
 * @brief Global bitmask array of a maze for testing.
 *
 */
static const uint16_t g_bitmask_array[GRID_ROWS * GRID_COLS] = {
    0x2, 0xE, 0xA, 0xC, 0x4, // Top Row
    0x6, 0xB, 0xC, 0x3, 0x9, // 2nd row
    0x3, 0x8, 0x7, 0x8, 0x4, // 3rd row
    0x4, 0x4, 0x7, 0xA, 0xD, // 4th row
    0x3, 0xB, 0x9, 0x2, 0x9  // last row
};

/**
 * @brief Global true grid for testing.
 *
 */
volatile grid_t g_true_grid
    = { .p_grid_array = NULL, .rows = GRID_ROWS, .columns = GRID_COLS };

// Test function prototypes.
//
static int test_initialise_empty_maze_nowall(void);
static int test_floodfill(void);

/**
 * @brief Runs the tests for the floodfill algorithm.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int 0 if successful, -1 otherwise.
 */
int
floodfill_tests (int argc, char *argv[])
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
            ret_val = test_initialise_empty_maze_nowall();
            break;
        case 2:
            ret_val = test_floodfill();
            break;
        default:
            printf("Invalid choice. Terminating.\n");
            ret_val = -1;
            break;
    }

    return ret_val;
}

/**
 * @brief Unit test for the initialise_empty_maze_nowall function.
 *
 * @return int 0 if successful, -1 otherwise.
 *
 * @NOTE: This test is not complete.
 */
static int
test_initialise_empty_maze_nowall (void)
{
    return 0;
}

/**
 * @brief Explores the current node. A placeholder for the explore function.
 *
 * @param p_grid Pointer to the maze.
 * @param p_navigator Pointer to the navigator.
 * @param direction Cardinal direction to explore.
 * @return uint16_t Bitmask of the walls.
 */
static uint16_t
explore_current_node (grid_t              *p_grid,
                      navigator_state_t   *p_navigator,
                      cardinal_direction_t direction)
{
    grid_cell_t *p_current_node = p_navigator->p_current_node;

    uint8_t bitmask
        = g_bitmask_array[p_current_node->coordinates.y * p_grid->columns
                          + p_current_node->coordinates.x];

    bitmask = 0xF - bitmask;

    p_navigator->orientation = direction;
    navigator_modify_walls(p_grid, p_navigator, bitmask, true, false);

    // char *p_maze_string = get_maze_string(p_grid);
    // printf("%s\n", p_maze_string);
    // free(p_maze_string);

    return bitmask;
}

/**
 * @brief Moves the navigator to the next node. Placeholder for robot movement.
 *
 * @param p_navigator Pointer to the navigator.
 * @param direction Cardinal direction to move.
 */
static void
move_navigator (navigator_state_t *p_navigator, cardinal_direction_t direction)
{
    p_navigator->p_current_node
        = p_navigator->p_current_node->p_next[direction];
    p_navigator->orientation = direction;
}

/**
 * @brief Unit test for the floodfill algorithm.
 *
 * @return int 0 if successful, -1 otherwise.
 *
 * @NOTE: This test is not complete.
 */
static int
test_floodfill (void)
{
    // Initialise the true grid and the maze.
    //
    g_true_grid = create_maze(GRID_ROWS, GRID_COLS);

    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);
    initialise_empty_maze_nowall(&maze);
    maze_gap_bitmask_t gap_bitmask = { .p_bitmask = (uint16_t *)g_bitmask_array,
                                       .rows      = GRID_ROWS,
                                       .columns   = GRID_COLS };

    deserialise_maze(&g_true_grid, &gap_bitmask);

    point_t start_point = { 0, 4 };
    point_t end_point   = { 4, 0 };

    grid_cell_t *p_start = get_cell_at_coordinates(&maze, &start_point);
    grid_cell_t *p_end   = get_cell_at_coordinates(&maze, &end_point);

    navigator_state_t navigator = { p_start, p_start, p_end, NORTH };

    explore_func_t   p_explore_func   = &explore_current_node;
    move_navigator_t p_move_navigator = &move_navigator;

    map_maze(
        &maze, p_start, p_end, &navigator, p_explore_func, p_move_navigator);

    // TODO(chris): Check whether the path is correct.
    return 0;
}

// End of file tests/floodfill_tests.c
