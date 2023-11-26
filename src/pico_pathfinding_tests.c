/**
 * @file pico_pathfinding_tests.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the mapping and pathfinding tests to be run on the
 * Pico depending on the input over serial.
 * @version 0.1
 * @date 2023-11-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/platform.h"
#include "maze.h"
#include "dfs.h"
#include "floodfill.h"
#include "a_star.h"

// Definitions.
// ----------------------------------------------------------------------------
//
#ifndef NDEBUG
/**
 * @def PICO_DEBUG_MALLOC
 * @brief Enables malloc debug.
 */
#define PICO_DEBUG_MALLOC 1
/**
 * @def DEBUG_PRINT(...)
 * @brief Debug print macro. Only prints if NDEBUG is not defined.
 * @param ... Variable arguments to be printed.
 * @note This macro is only defined if NDEBUG is not defined.
 */
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

// Type definitions.
// ----------------------------------------------------------------------------
//
typedef enum
{
    GRID_ROWS        = 6,    ///< Number of rows in the grid.
    GRID_COLS        = 4,    ///< Number of columns in the grid.
    TEST_BUFFER_SIZE = 2048u ///< Size of the test buffer for serialisation.
} constants_t;

// Global variables.
// ----------------------------------------------------------------------------
//

static const uint16_t g_bitmask_array_north[GRID_ROWS * GRID_COLS] = {
    0x6, 0xE, 0xC, 0x4, // First row.
    0x5, 0x1, 0x3, 0x9, // Second row.
    0x7, 0xA, 0xA, 0x8, // Third row.
    0x5, 0x6, 0xA, 0xC, // Fourth row.
    0x3, 0xD, 0x4, 0x1, // Fifth row.
    0x2, 0xB, 0xB, 0x8  // Last row.
};

/**
 * @brief Global bitmask of the array rotated 180 degrees.
 *
 */
static const uint16_t g_bitmask_array_south[GRID_ROWS * GRID_COLS] = {
    0x2, 0xE, 0xE, 0x8, // First row.
    0x4, 0x1, 0x7, 0xC, // Second row.
    0x3, 0xA, 0x9, 0x5, // Third row.
    0x2, 0xA, 0xA, 0xD, // Fourth row.
    0x6, 0xC, 0x4, 0x5, // Fifth row.
    0x1, 0x3, 0xB, 0x9  // Last row.
};

static const maze_point_t g_start_point = { 2, 5 }; // Start point is at (2, 5).
static const maze_point_t g_end_point   = { 1, 0 }; // End point is at (1, 0).

static uint16_t *gp_bitmask_array = NULL; // Pointer to the global bitmask array
                                          // that is set by the test functions.

// Private function prototypes.
// ----------------------------------------------------------------------------
//
static uint16_t explore_current_node(maze_grid_t              *p_grid,
                                     maze_navigator_state_t   *p_navigator,
                                     maze_cardinal_direction_t direction);
static void     move_navigator(maze_navigator_state_t   *p_navigator,
                               maze_cardinal_direction_t direction);
static void     map_maze(maze_grid_t            *p_grid,
                         const uint16_t         *p_bitmask_array,
                         maze_navigator_state_t *p_navigator);
static bool     is_maze_correct(maze_gap_bitmask_t map_bitmask,
                                maze_gap_bitmask_t true_map_bitmask);
static void     initialise_variables(const uint16_t         *p_bitmask,
                                     maze_grid_t            *p_maze,
                                     maze_navigator_state_t *p_navigator);
static int      test_mapping(const uint16_t *p_bitmask_array);
static int      test_navigation(const uint16_t *p_bitmask_array);
static int      test_combined(const uint16_t *p_bitmask_array);

// Test function prototypes.
// ----------------------------------------------------------------------------
//
static int test_mapping_northwards(void);
static int test_navigation_northwards(void);
static int test_combined_northwards(void);
static int test_mapping_southwards(void);
static int test_navigation_southwards(void);
static int test_combined_southwards(void);
static int test_compression(void);
static int test_navigator_serialisation(void);
static int test_path_serialisation(void);
static int test_combined_serialisation(void);
static int test_relative_direction(void);

int
main (void)
{
    // Initialise USB serial.
    //
    stdio_init_all();

    // Sleep 5s to allow the user to open the serial terminal.
    //
    sleep_ms(5000);

    for (;;)
    {

        printf(
            "Enter 1 to test mapping, 2 to test navigation, 3 to test combined "
            "(north)\n"
            "Enter 4 to test mapping, 5 to test navigation, 6 to test combined "
            "(south)\n");
        int choice = 0;
        scanf("%d", &choice);
        int ret_val = 0;

        switch (choice)
        {
            case 1:
                ret_val = test_mapping_northwards();
                break;
            case 2:
                ret_val = test_navigation_northwards();
                break;
            case 3:
                ret_val = test_combined_northwards();
                break;
            case 4:
                ret_val = test_mapping_southwards();
                break;
            case 5:
                ret_val = test_navigation_southwards();
                break;
            case 6:
                ret_val = test_combined_southwards();
                break;
            default:
                printf("Invalid choice.\n");
                ret_val = -1;
                break;
        }

        switch (ret_val)
        {
            case 0:
                printf("Test passed\n");
                break;
            case -1:
                printf("Test failed\n");
                break;
            default:
                printf("Invalid return value\n");
                break;
        }
    }

    return 0;
}

// Test functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Tests the mapping in the northwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_mapping_northwards (void)
{
    return test_mapping(g_bitmask_array_north);
}

/**
 * @brief Tests the navigation in the northwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_navigation_northwards (void)
{
    return test_navigation(g_bitmask_array_north);
}

/**
 * @brief Tests the combined mapping and navigation in the northwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_combined_northwards (void)
{
    return test_combined(g_bitmask_array_north);
}

/**
 * @brief Tests the mapping in the southwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_mapping_southwards (void)
{
    return test_mapping(g_bitmask_array_south);
}

/**
 * @brief Tests the navigation in the southwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_navigation_southwards (void)
{
    return test_navigation(g_bitmask_array_south);
}

/**
 * @brief Tests the combined mapping and navigation in the southwards direction.
 *
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_combined_southwards (void)
{
    return test_combined(g_bitmask_array_south);
}

// Private functions.
// ----------------------------------------------------------------------------
//
/**
 * @brief Explores the current node. Used by @ref dfs_depth_first_search.
 *
 * @param p_grid Pointer to the grid.
 * @param p_navigator Pointer to the navigator state.
 * @param direction Cardinal direction of the node to explore.
 * @return uint16_t Bitmask of the node.
 */
static uint16_t
explore_current_node (maze_grid_t              *p_grid,
                      maze_navigator_state_t   *p_navigator,
                      maze_cardinal_direction_t direction)
{
    maze_grid_cell_t *p_current_node = p_navigator->p_current_node;
    p_current_node->is_visited       = true;

    uint8_t bitmask
        = gp_bitmask_array[p_current_node->coordinates.y * p_grid->columns
                           + p_current_node->coordinates.x];

    bitmask = 0xF - bitmask;

    p_navigator->orientation = direction;

    char *p_maze_str = maze_get_string(p_grid);
    maze_insert_nav_str(p_grid, p_navigator, p_maze_str);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);

    return bitmask;
}

/**
 * @brief Moves the navigator in the given direction. Used by @ref
 * dfs_depth_first_search.
 *
 * @param p_navigator Pointer to the navigator state.
 * @param direction Cardinal direction to move the navigator.
 */
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

/**
 * @brief Maps the maze given a ground truth bitmask array.
 *
 * @param p_grid Pointer to the maze grid.
 * @param p_bitmask_array Pointer to the true bitmask array.
 * @param p_navigator Pointer to the navigator state.
 */
static void
map_maze (maze_grid_t            *p_grid,
          const uint16_t         *p_bitmask_array,
          maze_navigator_state_t *p_navigator)
{
    const maze_gap_bitmask_t gap_bitmask = { .p_bitmask = p_bitmask_array,
                                             .rows      = GRID_ROWS,
                                             .columns   = GRID_COLS };
    maze_grid_t              true_grid   = maze_create(GRID_ROWS, GRID_COLS);
    maze_deserialise(&true_grid, &gap_bitmask);
    char *p_true_maze_str = maze_get_string(&true_grid);
    printf("%s\n\n", p_true_maze_str);
    free(p_true_maze_str);

    floodfill_init_maze_nowall(p_grid);

    floodfill_explore_func_t   p_explore_func   = &explore_current_node;
    floodfill_move_navigator_t p_move_navigator = &move_navigator;

    dfs_depth_first_search(p_grid,
                           p_navigator->p_start_node,
                           p_navigator,
                           p_explore_func,
                           p_move_navigator);

    char *p_maze_str = maze_get_string(p_grid);
    maze_insert_nav_str(p_grid, p_navigator, p_maze_str);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);
}

/**
 * @brief Checks whether the mapped maze is correct.
 *
 * @param map_bitmask Bitmask of the mapped maze.
 * @param true_map_bitmask Bitmask of the true state of the maze.
 * @return true Mapped maze is correct.
 * @return false Mapped maze is not correct.
 */
static bool
is_maze_correct (maze_gap_bitmask_t map_bitmask,
                 maze_gap_bitmask_t true_map_bitmask)
{
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
                printf("Found bitmask: %u\n", bitmask);

                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Initialises the variables for the tests. Useful as there are 2
 * directions the tests can be run in.
 *
 * @param p_bitmask Pointer to the bitmask array (direction dependent).
 * @param p_maze Pointer to the maze grid.
 * @param p_navigator Pointer to the navigator state.
 */
static void
initialise_variables (const uint16_t         *p_bitmask,
                      maze_grid_t            *p_maze,
                      maze_navigator_state_t *p_navigator)
{
    printf("Initialising variables\n");
    gp_bitmask_array = p_bitmask;
    floodfill_init_maze_nowall(p_maze);

    maze_grid_cell_t *p_start = maze_get_cell_at_coords(p_maze, &g_start_point);
    maze_grid_cell_t *p_end   = maze_get_cell_at_coords(p_maze, &g_end_point);

    p_navigator->orientation    = MAZE_NORTH;
    p_navigator->p_current_node = p_start;
    p_navigator->p_start_node   = p_start;
    p_navigator->p_end_node     = p_end;
}

/**
 * @brief Tests mapping of the maze.
 *
 * @param p_bitmask_array Pointer to the true bitmask array (direction
 * dependent).
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_mapping (const uint16_t *p_bitmask_array)
{
    printf("Testing mapping\n");
    int                    ret_val = 0;
    maze_grid_t            maze    = maze_create(GRID_ROWS, GRID_COLS);
    maze_navigator_state_t navigator;

    initialise_variables(p_bitmask_array, &maze, &navigator);

    map_maze(&maze, gp_bitmask_array, &navigator);

    maze_gap_bitmask_t map_bitmask      = maze_serialise(&maze);
    maze_gap_bitmask_t true_map_bitmask = { .p_bitmask = gp_bitmask_array,
                                            .rows      = GRID_ROWS,
                                            .columns   = GRID_COLS };

    bool is_test_passing = is_maze_correct(map_bitmask, true_map_bitmask);

    ret_val = is_test_passing ? 0 : -1;
    maze_destroy(&maze);
    return ret_val;
}

/**
 * @brief Tests navigation of the maze.
 *
 * @param p_bitmask_array Pointer to the true bitmask array (direction
 * dependent).
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_navigation (const uint16_t *p_bitmask_array)
{
    printf("Testing navigation\n");
    int                    ret_val = 0;
    maze_grid_t            maze    = maze_create(GRID_ROWS, GRID_COLS);
    maze_navigator_state_t navigator;

    initialise_variables(p_bitmask_array, &maze, &navigator);
    maze_gap_bitmask_t true_map_bitmask = { .p_bitmask = p_bitmask_array,
                                            .rows      = GRID_ROWS,
                                            .columns   = GRID_COLS };
    maze_deserialise(&maze, &true_map_bitmask);

    a_star(&maze, navigator.p_start_node, navigator.p_end_node);
    a_star_path_t *p_path = a_star_get_path(navigator.p_end_node);

    if (NULL == p_path)
    {
        printf("Path is NULL\n");
        ret_val = -1;
        goto end;
    }

    char *maze_str = a_star_get_path_str(&maze, p_path);
    printf("%s\n\n", maze_str);

end:
    free(maze_str);
    navigator.p_current_node = NULL;
    navigator.p_start_node   = NULL;
    navigator.p_end_node     = NULL;
    maze_destroy(&maze);

    return ret_val;
}

/**
 * @brief Tests both the mapping and navigation of the maze.
 *
 * @param p_bitmask_array Pointer to the true bitmask array (direction
 * dependent).
 * @return int 0 if the test passes, -1 otherwise.
 */
static int
test_combined (const uint16_t *p_bitmask_array)
{
    printf("Testing combined\n");
    int                    ret_val = 0;
    maze_grid_t            maze    = maze_create(GRID_ROWS, GRID_COLS);
    maze_navigator_state_t navigator;

    printf("Initialising variables\n");
    initialise_variables(p_bitmask_array, &maze, &navigator);

    printf("Mapping maze\n");
    map_maze(&maze, gp_bitmask_array, &navigator);

    maze_gap_bitmask_t map_bitmask      = maze_serialise(&maze);
    maze_gap_bitmask_t true_map_bitmask = { .p_bitmask = gp_bitmask_array,
                                            .rows      = GRID_ROWS,
                                            .columns   = GRID_COLS };

    printf("Checking mapping\n");
    bool is_test_passing = is_maze_correct(map_bitmask, true_map_bitmask);
    if (!is_test_passing)
    {
        ret_val = -1;
        printf("Failed mapping\n");
        goto end_mapping;
    }

    printf("Conducting navigation\n");
    a_star(&maze, navigator.p_current_node, navigator.p_start_node);
    a_star_path_t *p_path = a_star_get_path(navigator.p_start_node);
    for (size_t idx = 1; p_path->length > idx; idx++)
    {
        char *p_maze_str = a_star_get_path_str(&maze, p_path);
        maze_insert_nav_str(&maze, &navigator, p_maze_str);
        printf("Path Step %u:\n%s\n\n", idx, p_maze_str);
        free(p_maze_str);

        maze_grid_cell_t          next_node      = p_path->p_path[idx];
        maze_cardinal_direction_t next_direction = maze_get_dir_from_to(
            &navigator.p_current_node->coordinates, &next_node.coordinates);

        move_navigator(&navigator, next_direction);
    }

    if (navigator.p_current_node != navigator.p_start_node)
    {
        ret_val = -1;
        printf("Navigator did not return to start\n");
        goto end_return_to_start;
    }

    printf("Conducting navigation\n");
    a_star(&maze, navigator.p_current_node, navigator.p_end_node);
    p_path           = a_star_get_path(navigator.p_end_node);
    char *p_maze_str = a_star_get_path_str(&maze, p_path);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);

    printf("Moving navigator\n");
    for (size_t idx = 1; p_path->length > idx; idx++)
    {
        char *p_maze_str = a_star_get_path_str(&maze, p_path);
        maze_insert_nav_str(&maze, &navigator, p_maze_str);
        printf("Path Step %u:\n%s\n\n", idx, p_maze_str);
        free(p_maze_str);
        maze_grid_cell_t          next_node      = p_path->p_path[idx];
        maze_cardinal_direction_t next_direction = maze_get_dir_from_to(
            &navigator.p_current_node->coordinates, &next_node.coordinates);

        move_navigator(&navigator, next_direction);
    }
    p_maze_str = a_star_get_path_str(&maze, p_path);
    maze_insert_nav_str(&maze, &navigator, p_maze_str);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);

    if (navigator.p_end_node != navigator.p_current_node)
    {
        ret_val = -1;
        printf("Navigator did not reach end\n");
        printf("Navigator is at (%u, %u)\n",
               navigator.p_current_node->coordinates.x,
               navigator.p_current_node->coordinates.y);
        goto end_return_to_start;
    }

end_return_to_start:
    free(p_path->p_path);
    free(p_path);

end_mapping:
    maze_destroy(&maze);

    return ret_val;
}