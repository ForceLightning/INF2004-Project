#ifndef A_STAR_H
#define A_STAR_H

#include <stdint.h>
#include "binary_heap.h"
#include "maze.h"

// Structures.
//
typedef struct path
{
    uint32_t     length;
    grid_cell_t *p_path;
} path_t;

// Public function prototypes.
//
void a_star(grid_t grid, grid_cell_t *p_start_node, grid_cell_t *p_end_node);
path_t  *get_path(grid_cell_t *p_end_node);
char    *get_path_string(grid_t *p_grid, path_t *p_path);
#endif // A_STAR_H

// End of pathfinding/a_star.h
