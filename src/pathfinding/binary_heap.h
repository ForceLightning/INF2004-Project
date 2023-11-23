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

// Definitions.
// ----------------------------------------------------------------------------
//

#ifndef NDEBUG
#include <stdio.h>

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
 * @typedef heap_node_t
 * @brief This struct contains basic information about a node in a priority
 * queue implemented with a binary heap.
 *
 * @property priority Priority of the node. This is the F-value of the node.
 * @property p_maze_node Pointer to a node in the maze.
 *
 */
typedef struct binary_heap_node
{
    uint16_t priority; ///< Priority of the node. This is the F-value of the
                       ///< node.
    maze_grid_cell_t *p_maze_node; ///< Pointer to a node in the maze.

} binary_heap_node_t;

/**
 * @typedef binary_heap_t
 * @brief Struct containing the binary heap.
 *
 * @property p_array Pointer to the first element of the array.
 * @property capacity Maximum number of nodes that can be stored in the binary
 * heap.
 * @property size Current number of nodes in the binary heap.
 *
 * @warning The array is 0-indexed, so capacity and size are always 1 greater
 * than their respective index maximums.
 */
typedef struct binary_heap
{
    binary_heap_node_t *p_array; ///< Pointer to the first element of the array.
    uint16_t capacity; ///< Maximum number of nodes that can be stored in
                       ///< the binary heap.

    uint16_t size; ///< Current number of nodes in the binary heap.
} binary_heap_t;

// Public functions.
// ----------------------------------------------------------------------------
//

void binary_heapify_up(binary_heap_t *p_heap, uint16_t index);

void binary_heapify_down(binary_heap_t *p_heap, uint16_t index);

void binary_heap_insert(binary_heap_t    *p_heap,
            maze_grid_cell_t *p_maze_node,
            uint16_t          priority);

maze_grid_cell_t *binary_heap_delete_min(binary_heap_t *p_heap);

binary_heap_node_t binary_heap_peek(binary_heap_t *p_heap);

uint16_t binary_heap_get_node_idx(const binary_heap_t    *p_heap,
                           const maze_grid_cell_t *p_maze_node);

#endif // BINARY_HEAP_H

// End of pathfinding/binary_heap.h
