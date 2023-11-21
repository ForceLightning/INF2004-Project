#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "pathfinding/maze.h"
#include "pathfinding/floodfill.h"
#include "pathfinding/dfs.h"
#include "pathfinding/a_star.h"

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
 * @typedef constants_t
 * @brief This enum contains constants used in the tests.
 *
 */
typedef enum constants
{
    GRID_ROWS = 6, ///< Number of rows in the grid.
    GRID_COLS = 4  ///< Number of columns in the grid.
} constants_t;

// Global variables.
// ----------------------------------------------------------------------------
//

/**
 * @brief Global bitmask array of a maze for testing.
 *
 */
static const uint16_t g_bitmask_array_north[GRID_ROWS * GRID_COLS] = {
    0x6, 0xE, 0xC, 0x4, // First row.
    0x5, 0x1, 0x3, 0x9, // Second row.
    0x7, 0xA, 0xA, 0x8, // Third row.
    0x5, 0x6, 0xA, 0xC, // Fourth row.
    0x3, 0xD, 0x4, 0x1, // Fifth row.
    0x2, 0xB, 0xB, 0x8  // Last row.
};

static const uint16_t g_bitmask_array_south[GRID_ROWS * GRID_COLS] = {
    0x2, 0xE, 0xE, 0x8, // First row.
    0x4, 0x1, 0x7, 0xC, // Second row.
    0x3, 0xA, 0x9, 0x5, // Third row.
    0x2, 0xA, 0xA, 0xD, // Fourth row.
    0x6, 0xC, 0x4, 0x5, // Fifth row.
    0x1, 0x3, 0xB, 0x9  // Last row.
};

static const maze_point_t start_point = { 2, 5 }; // Start point is at (2, 5).
static const maze_point_t end_point   = { 1, 0 }; // End point is at (1, 0).

static uint16_t *gp_bitmask_array = NULL;

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

// Private function prototypes.
// ----------------------------------------------------------------------------
//
static uint16_t explore_current_node(maze_grid_t              *p_grid,
                                     maze_navigator_state_t   *p_navigator,
                                     maze_cardinal_direction_t direction);

static void move_navigator(maze_navigator_state_t   *p_navigator,
                           maze_cardinal_direction_t direction);

static void map_maze(maze_grid_t            *p_grid,
                     const uint16_t         *p_bitmask_array,
                     maze_navigator_state_t *p_navigator);

static bool is_maze_correct(maze_gap_bitmask_t map_bitmask,
                            maze_gap_bitmask_t true_map_bitmask);

static void initialise_variables(uint16_t               *p_bitmask,
                                 maze_grid_t            *p_maze,
                                 maze_navigator_state_t *p_navigator);

static int test_mapping(const uint16_t *p_bitmask_array);
static int test_navigation(const uint16_t *p_bitmask_array);
static int test_combined(const uint16_t *p_bitmask_array);

int
navigation_tests (int argc, char *argv[])
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
        case 7:
            ret_val = test_compression();
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
test_mapping_northwards (void)
{
    return test_mapping(g_bitmask_array_north);
}

static int
test_navigation_northwards (void)
{
    return test_navigation(g_bitmask_array_north);
}

static int
test_combined_northwards (void)
{
    return test_combined(g_bitmask_array_north);
}

static int
test_mapping_southwards (void)
{
    return test_mapping(g_bitmask_array_south);
}

static int
test_navigation_southwards (void)
{
    return test_navigation(g_bitmask_array_south);
}

static int
test_combined_southwards (void)
{
    return test_combined(g_bitmask_array_south);
}

static int
test_compression (void)
{
    int ret_val = 0;

    maze_grid_t maze = maze_create(GRID_ROWS, GRID_COLS);

    maze_gap_bitmask_t true_map_bitmask = { .p_bitmask = g_bitmask_array_north,
                                            .rows      = GRID_ROWS,
                                            .columns   = GRID_COLS };
    maze_deserialise(&maze, &true_map_bitmask);

    maze_gap_bitmask_t map_bitmask = maze_serialise(&maze);

    maze_bitmask_compressed_t *p_compressed_bitmask
        = malloc(sizeof(maze_bitmask_compressed_t) * GRID_ROWS * GRID_COLS);

    uint8_t *p_buffer = malloc(sizeof(uint8_t) * 2048u);
    memset(p_buffer, 0, sizeof(uint8_t) * 2048u);

    maze_serialised_to_buffer(&map_bitmask, p_buffer, 2048u);

    char *p_compressed_str = malloc(sizeof(char) * 4096u);
    memset(p_compressed_str, 0, sizeof(char) * 4096u);
    
    uint16_t compressed_size = 4 + (GRID_ROWS * GRID_COLS) / 2 + (GRID_ROWS * GRID_COLS) % 2;

    for (size_t idx = 0; compressed_size > idx; idx++)
    {
        sprintf(p_compressed_str + idx * 2, "%2X", p_buffer[idx]);
    }

    printf("Compressed string:\n%s\n", p_compressed_str);
    
    free(p_buffer);
    free(p_compressed_str);
    
    return ret_val;
}

// Private function definitions.
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
        = gp_bitmask_array[p_current_node->coordinates.y * p_grid->columns
                           + p_current_node->coordinates.x];

    bitmask = 0xF - bitmask;

    p_navigator->orientation = direction;
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

static void
initialise_variables (uint16_t               *p_bitmask,
                      maze_grid_t            *p_maze,
                      maze_navigator_state_t *p_navigator)
{
    gp_bitmask_array = p_bitmask;
    floodfill_init_maze_nowall(p_maze);

    maze_grid_cell_t *p_start = maze_get_cell_at_coords(p_maze, &start_point);
    maze_grid_cell_t *p_end   = maze_get_cell_at_coords(p_maze, &end_point);

    p_navigator->orientation    = NORTH;
    p_navigator->p_current_node = p_start;
    p_navigator->p_start_node   = p_start;
    p_navigator->p_end_node     = p_end;
}

static int
test_mapping (const uint16_t *p_bitmask_array)
{
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

static int
test_navigation (const uint16_t *p_bitmask_array)
{
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

static int
test_combined (const uint16_t *p_bitmask_array)
{
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
    if (!is_test_passing)
    {
        ret_val = -1;
        printf("Failed mapping\n");
        goto end_mapping;
    }

    a_star(&maze, navigator.p_current_node, navigator.p_start_node);
    a_star_path_t *p_path = a_star_get_path(navigator.p_start_node);
    for (size_t idx = 1; p_path->length > idx; idx++)
    {
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

    a_star(&maze, navigator.p_current_node, navigator.p_end_node);
    p_path           = a_star_get_path(navigator.p_end_node);
    char *p_maze_str = a_star_get_path_str(&maze, p_path);
    printf("%s\n\n", p_maze_str);
    free(p_maze_str);

    for (size_t idx = 1; p_path->length > idx; idx++)
    {
        maze_grid_cell_t          next_node = p_path->p_path[idx];
        maze_cardinal_direction_t next_direction
            = maze_get_dir_from_to(navigator.p_current_node, &next_node);

        move_navigator(&navigator, next_direction);
    }

    if (navigator.p_end_node != navigator.p_current_node)
    {
        ret_val = -1;
        printf("Navigator did not reach end\n");
        printf("Navigator is at (%llu, %llu)\n",
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

// End of file tests/navigation_tests.c
