#ifndef A_STAR_H
#define A_STAR_H

#include <stdint.h>
#include "binary_heap.h"

void a_star(grid_t grid, grid_cell_t *p_start_node, grid_cell_t *p_end_node);
uint32_t manhattan_distance(point_t *point_a, point_t *point_b);
#endif // A_STAR_H

// End of pathfinding/a_star.h
