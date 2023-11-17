#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pathfinding/maze.h"
#include "pathfinding/dfs.h"

#ifndef DEBUG
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
 * @typedef constants_t
 * @brief This enum contains constants used in the tests.
 *
 */
typedef enum constants
{
    GRID_ROWS = 5, ///< Number of rows in the grid.
    GRID_COLS = 5  ///< Number of columns in the grid.
} constants_t;

// Global variables.
// ----------------------------------------------------------------------------
//

/**
 * @brief Global bitmask array of a maze for testing.
 *
 */
static const uint8_t g_bitmask_array[GRID_ROWS * GRID_COLS] = {
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
volatile grid_t g_dfs_true_grid
    = { .p_grid_array = NULL, .rows = GRID_ROWS, .columns = GRID_COLS };

// Test function prototypes.
// ----------------------------------------------------------------------------
//

static int test_depth_first_search(void);

// Private function prototypes.
// ----------------------------------------------------------------------------
//
static uint16_t explore_current_node(grid_t              *p_grid,
                                     navigator_state_t   *p_navigator,
                                     cardinal_direction_t direction);
static void     move_navigator(navigator_state_t   *p_navigator,
                               cardinal_direction_t direction);

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
        default:
            printf("Invalid choice. Terminating.\n");
            ret_val = -1;
            break;
    }

    return ret_val;
}

static int
test_depth_first_search (void)
{
    g_dfs_true_grid = create_maze(GRID_ROWS, GRID_COLS);

    grid_t maze = create_maze(GRID_ROWS, GRID_COLS);
    initialise_empty_maze_nowall(&maze);
    maze_gap_bitmask_t gap_bitmask = { .p_bitmask = (uint16_t *)g_bitmask_array,
                                       .rows      = GRID_ROWS,
                                       .columns   = GRID_COLS };
    deserialise_maze(&g_dfs_true_grid, &gap_bitmask);

    // Initialise the navigator.
    //
    point_t start_point = { 0, 4 };
    point_t end_point   = { 4, 0 };

    grid_cell_t *p_start = get_cell_at_coordinates(&maze, &start_point);
    grid_cell_t *p_end   = get_cell_at_coordinates(&maze, &end_point);

    navigator_state_t navigator = { p_start, p_start, NULL, NORTH };

    explore_func_t   p_explore_func   = &explore_current_node;
    move_navigator_t p_move_navigator = &move_navigator;

    dfs_depth_first_search(
        &maze, p_start, &navigator, p_explore_func, p_move_navigator);

    // Check that the maze is correct.
    //
    maze_gap_bitmask_t true_map_bitmask = serialise_maze(&g_dfs_true_grid);
    maze_gap_bitmask_t map_bitmask      = serialise_maze(&maze);

    for (uint16_t row; GRID_ROWS > row; row++)
    {
        for (uint16_t col; GRID_COLS > col; col++)
        {
            uint16_t true_bitmask
                = true_map_bitmask.p_bitmask[row * GRID_COLS + col];
            uint16_t bitmask = map_bitmask.p_bitmask[row * GRID_COLS + col];

            if (true_bitmask != bitmask)
            {
                printf("Maze is not correct.\n");
                return -1;
            }
        }
    }
    return 0;
}

// Private functions.
// ----------------------------------------------------------------------------
//
static uint16_t
explore_current_node (grid_t              *p_grid,
                      navigator_state_t   *p_navigator,
                      cardinal_direction_t direction)
{
    grid_cell_t *p_current_node = p_navigator->p_current_node;
    p_current_node->is_visited  = true;

    uint8_t bitmask
        = g_bitmask_array[p_current_node->coordinates.y * p_grid->columns
                          + p_current_node->coordinates.x];

    bitmask = 0xF - bitmask;

    p_navigator->orientation = direction;
    navigator_modify_walls(p_grid, p_navigator, bitmask, true, false);
}

static void
move_navigator (navigator_state_t *p_navigator, cardinal_direction_t direction)
{
    p_navigator->orientation = direction;
    grid_cell_t *p_next_node = p_navigator->p_current_node->p_next[direction];
    if (NULL == p_next_node->p_came_from)
    {
        p_next_node->p_came_from = p_navigator->p_current_node;
    }
    p_navigator->p_current_node = p_next_node;
}