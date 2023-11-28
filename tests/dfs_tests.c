/**
 * @file dfs_tests.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the functions that test the depth first search
 * algorithm with a simulated navigator.
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pathfinding/binary_heap.h"
#include "pathfinding/floodfill.h"
#include "pathfinding/maze.h"
#include "pathfinding/dfs.h"

// Definitions.
// ----------------------------------------------------------------------------
//

#ifndef NDEBUG
/**
 * @def DEBUG_PRINT(...)
 * @brief Debug print macro. Only prints if NDEBUG is not defined.
 * @param ... Variable arguments to be printed.
 *
 */
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @brief This enum contains constants used in the tests.
 */
typedef enum
{
    GRID_ROWS = 5, ///< Number of rows in the grid.
    GRID_COLS = 5  ///< Number of columns in the grid.
} constants_t;

// Global variables.
// ----------------------------------------------------------------------------
//

/**
 * @brief Global bitmask array of a maze for testing.
 */
static const uint16_t g_bitmask_array[GRID_ROWS * GRID_COLS] = {
    0x2, 0xE, 0xA, 0xC, 0x4, // Top Row
    0x6, 0xB, 0xC, 0x3, 0x9, // 2nd row
    0x3, 0x8, 0x7, 0x8, 0x4, // 3rd row
    0x4, 0x4, 0x7, 0xA, 0xD, // 4th row
    0x3, 0xB, 0x9, 0x2, 0x9  // last row
};

// Test function prototypes.
// ----------------------------------------------------------------------------
//

static int test_depth_first_search(void);
static int test_all_reachable_visisted(void);

// Private function prototypes.
// ----------------------------------------------------------------------------
//
static uint16_t explore_current_node(maze_grid_t              *p_grid,
                                     maze_navigator_state_t   *p_navigator,
                                     maze_cardinal_direction_t direction);
static void     move_navigator(maze_navigator_state_t   *p_navigator,
                               maze_cardinal_direction_t direction);

int
dfs_tests (int argc, char *argv[])
{
    int default_choice = 1; // Default choice for the test to run.
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
            ret_val = test_depth_first_search();
            break;
        case 2:
            ret_val = test_all_reachable_visisted();
            break;
        default:
            printf("Invalid choice. Terminating.\n");
            ret_val = -1;
            break;
    }

    return ret_val;
}

// Test function definitions.
// ----------------------------------------------------------------------------
//

static int
test_depth_first_search (void)
{
    maze_grid_t true_grid = maze_create(GRID_ROWS, GRID_COLS);

    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);
    floodfill_init_maze_nowall(&maze);
    maze_gap_bitmask_t gap_bitmask = { .p_bitmask = (uint16_t *)g_bitmask_array,
                                       .rows      = GRID_ROWS,
                                       .columns   = GRID_COLS };
    maze_deserialise(&true_grid, &gap_bitmask);

    char *p_true_maze_str = maze_get_string(&true_grid);
    printf("%s\n\n", p_true_maze_str);

    // Initialise the navigator.
    //
    maze_point_t start_point = { 0, 4 };
    maze_point_t end_point   = { 4, 0 };

    maze_grid_cell_t *p_start = maze_get_cell_at_coords(&maze, &start_point);
    maze_grid_cell_t *p_end   = maze_get_cell_at_coords(&maze, &end_point);

    maze_navigator_state_t navigator = { p_start, p_start, NULL, MAZE_NORTH };

    floodfill_explore_func_t   p_explore_func   = &explore_current_node;
    floodfill_move_navigator_t p_move_navigator = &move_navigator;

    dfs_depth_first_search(
        &maze, p_start, &navigator, p_explore_func, p_move_navigator);

    // Check that the maze is correct.
    //
    maze_gap_bitmask_t true_map_bitmask = maze_serialise(&true_grid);
    maze_gap_bitmask_t map_bitmask      = maze_serialise(&maze);

    for (size_t row = 0; GRID_ROWS > row; row++)
    {
        for (size_t col = 0; GRID_COLS > col; col++)
        {
            uint16_t true_bitmask
                = true_map_bitmask.p_bitmask[row * GRID_COLS + col];
            uint16_t bitmask = map_bitmask.p_bitmask[row * GRID_COLS + col];

            if (true_bitmask != bitmask)
            {
                printf("Maze is not correct at row %llu, col %llu\n", row, col);
                printf("True bitmask: %u\n", true_bitmask);
                p_true_maze_str = maze_get_string(&true_grid);
                printf("%s\n\n", p_true_maze_str);
                printf("Found bitmask: %u\n", bitmask);
                char *p_maze_str = maze_get_string(&maze);
                printf("%s\n\n", p_maze_str);

                free(p_true_maze_str);
                free(p_maze_str);
                return -1;
            }
        }
    }
    return 0;
}

static int
test_all_reachable_visisted (void)
{
    int         ret_val   = 0;
    maze_grid_t true_grid = maze_create(GRID_ROWS, GRID_COLS);

    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);
    floodfill_init_maze_nowall(&maze);
    maze_gap_bitmask_t gap_bitmask = { .p_bitmask = (uint16_t *)g_bitmask_array,
                                       .rows      = GRID_ROWS,
                                       .columns   = GRID_COLS };
    maze_deserialise(&true_grid, &gap_bitmask);

    // Initialise the navigator.
    //
    maze_point_t start_point = { 0, 4 };
    maze_point_t end_point   = { 4, 0 };

    maze_grid_cell_t *p_start = maze_get_cell_at_coords(&maze, &start_point);
    maze_grid_cell_t *p_end   = maze_get_cell_at_coords(&maze, &end_point);

    maze_navigator_state_t navigator = { p_start, p_start, NULL, MAZE_NORTH };

    floodfill_explore_func_t   p_explore_func   = &explore_current_node;
    floodfill_move_navigator_t p_move_navigator = &move_navigator;

    for (size_t row = 0; GRID_ROWS > row; row++)
    {
        for (size_t col = 0; GRID_COLS > col; col++)
        {
            maze_grid_cell_t *p_cell
                = &maze.p_grid_array[row * GRID_COLS + col];
            p_cell->is_visited = true;
        }
    }

    binary_heap_t reachable_set
        = { .p_array = NULL, .size = 0, .capacity = GRID_ROWS * GRID_COLS };
    reachable_set.p_array
        = malloc(sizeof(maze_grid_cell_t) * reachable_set.capacity);

    binary_heap_insert(&reachable_set, p_start, 0);

    if (!dfs_is_all_reachable_visited(&maze, &navigator))
    {
        ret_val = -1;
        printf("Not all reachable nodes are visited.\n");
    }

    return ret_val;
}

// Private functions.
// ----------------------------------------------------------------------------
//
static uint16_t
explore_current_node (maze_grid_t              *p_grid,
                      maze_navigator_state_t   *p_navigator,
                      maze_cardinal_direction_t direction)
{
    maze_grid_cell_t *p_current_node = p_navigator->p_current_node;
    p_current_node->is_visited       = true;

    uint8_t bitmask
        = g_bitmask_array[p_current_node->coordinates.y * p_grid->columns
                          + p_current_node->coordinates.x];

    bitmask = MAZE_INVERT_BITMASK(bitmask);

    p_navigator->orientation = direction;
    maze_nav_modify_walls(p_grid, p_navigator, bitmask, true, false);
    char *p_maze_str = maze_get_string(p_grid);
    maze_insert_nav_str(p_grid, p_navigator, p_maze_str);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);
    return bitmask;
}

static void
move_navigator (maze_navigator_state_t   *p_navigator,
                maze_cardinal_direction_t direction)
{
    p_navigator->orientation = direction;
    maze_grid_cell_t *p_next_node
        = p_navigator->p_current_node->p_next[direction];
    if (NULL == p_next_node->p_came_from)
    {
        p_next_node->p_came_from = p_navigator->p_current_node;
    }
    p_navigator->p_current_node = p_next_node;
}