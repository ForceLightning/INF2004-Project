/**
 * @file binary_heap.h
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief This file contains the declarations for the binary min-heap used in
 * the a* algorithm.
 * @version 0.1
 * @date 2023-10-12
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <stdint.h>
#include "maze.h"

#ifndef NDEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#endif

/**
 * @brief This struct contains basic information about a node in a priority
 * queue implemented with a binary heap
 *
 * @param weight Weight of the node. This is the F-value of the node.
 * @param maze_node Pointer to a node in the maze.
 * @param p_next Pointer to the next node in the binary heap.
 */
typedef struct heap_node
{
    uint16_t     priority;
    grid_cell_t *p_maze_node;

} heap_node_t;

/**
 * @brief Struct containing the binary heap.
 *
 * @param p_array Pointer to the first element of the array.
 * @param capacity Maximum number of nodes that can be stored in the binary
 * heap.
 * @param size Current number of nodes in the binary heap.
 *
 * @WARNING: The array is 0-indexed, so capacity and size are always 1 greater
 * than their respective index maximums
 */
typedef struct binary_heap
{
    heap_node_t *p_array;
    uint16_t     capacity;
    uint16_t     size;
} binary_heap_t;

// Public function declarations for manipulation of the binary heap.
//
void insert(binary_heap_t *p_heap, grid_cell_t *p_maze_node, uint16_t priority);
grid_cell_t *delete_min(binary_heap_t *p_heap);
heap_node_t  peek(binary_heap_t *p_heap);

#endif // BINARY_HEAP_H

// End of file comment
