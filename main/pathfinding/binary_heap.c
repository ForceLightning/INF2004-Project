/**
 * @file binary_heap.c
 * @author Christopher Kok (chris@forcelightning.xyz)
 * @brief Source file for the binary min-heap used in the a* algorithm and
 * floodfill algorithm.
 * @version 0.1
 * @date 2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stddef.h>
#include <stdint.h>
#include "binary_heap.h"
#include "maze.h"

// Public functions.
// ----------------------------------------------------------------------------
//

/**
 * @brief Moves the node at the given index down the binary heap to maintain the
 * heap property. This in effect raises the node to the correct position.
 *
 * @param p_heap Pointer to the binary heap.
 * @param index Index of the node to be heapified up.
 */
void
binary_heapify_up (binary_heap_t *p_heap, uint16_t index)
{
    // @note These early returns are not necessary, but they make the code
    // easier to read.

    // Step 1: Check if the node is the root node.
    //
    if (0 == index)
    {
        return;
    }

    // Step 2: Get the parent index. In a binary heap, the parent index is
    // always (index - 1) / 2.
    //
    uint16_t parent_index = (index - 1) / 2;

    // Step 3: Check if the parent node has a lower or equal priority than the
    // current node. If it is, the heap property is satisfied and we can return.
    //
    if (p_heap->p_array[parent_index].priority
        <= p_heap->p_array[index].priority)
    {
        return;
    }

    // Otherwise, loop until the heap property is satisfied.
    //
    while (index > 0
           && (p_heap->p_array[parent_index].priority
               > p_heap->p_array[index].priority))
    {
        // Step 4: Swap the parent node with the current node.
        //
        binary_heap_node_t temp_node
            = p_heap->p_array[parent_index]; // Save the parent node.
        p_heap->p_array[parent_index] = p_heap->p_array[index]; // Swap.
        p_heap->p_array[index]        = temp_node;              // Swap.

        // Step 5: Heapify up the parent node.
        // Update the index to the parent's index and recalculate the node's new
        // parent's index.
        //
        index        = parent_index;    // Update the index.
        parent_index = (index - 1) / 2; // Recalculate the parent's index.
    }
}

/**
 * @brief Moves the node at the given index up the binary heap to maintain the
 * heap property. This is in effect lowers the node to the correct position.
 *
 * @param p_heap Pointer to the binary heap.
 * @param index Index of the node to be heapified down.
 */
void
binary_heapify_down (binary_heap_t *p_heap, uint16_t index)
{
    for (;;)
    {
        // Step 1: Calculate the left and right child indices. In a binary
        // heap, the indices are known to be 2 * index + 1..=2 for the left and
        // right respectively.
        //
        uint16_t left_index  = 2 * index + 1;  // Calculate the left index.
        uint16_t right_index = 2 * index + 2;  // Calculate the right index.
        uint16_t smallest_child_index = index; // Assume that the current node
                                               // is the smallest.

        // Step 2: Find the smallest value amongst the node and its children,
        // and assign the index to `smallest_child_index`.
        //
        if (left_index < p_heap->size
            && (p_heap->p_array[left_index].priority
                < p_heap->p_array[smallest_child_index].priority))
        {
            smallest_child_index = left_index;
        }

        if (right_index < p_heap->size
            && (p_heap->p_array[right_index].priority
                < p_heap->p_array[smallest_child_index].priority))
        {
            smallest_child_index = right_index;
        }

        // Step 3: If the smallest value is at the current index, then the heap
        // property is satisfied and we can break out of the loop.
        //
        if (smallest_child_index == index)
        {
            break;
        }

        // Step 4: Otherwise, swap the current node with the smallest child.
        //
        binary_heap_node_t temp_node = p_heap->p_array[index];
        p_heap->p_array[index]       = p_heap->p_array[smallest_child_index];
        p_heap->p_array[smallest_child_index] = temp_node;

        // Step 5: Update the index to the smallest child's index for the next
        // iteration.
        //
        index = smallest_child_index;
    }
}

/**
 * @brief Inserts a grid cell node into the binary heap with a given priority.
 *
 * @param p_heap Pointer to the binary min-heap.
 * @param p_maze_node Pointer to the grid cell node to be inserted.
 * @param priority Priority of the node to be inserted.
 */
void
binary_heap_insert (binary_heap_t    *p_heap,
                    maze_grid_cell_t *p_maze_node,
                    uint16_t          priority)
{
    // Step 1: Check if the heap is full.
    //
    if (p_heap->size == p_heap->capacity)
    {
        // If not in debug mode, don't print this message.
        DEBUG_PRINT("Heap is full!\n");
        return;
    }

    // Step 2: Create a new heap node.
    //
    binary_heap_node_t new_node;
    new_node.p_maze_node = p_maze_node;
    new_node.priority    = priority;

    // Step 3: Insert it at the end of the array.
    //
    p_heap->p_array[p_heap->size] = new_node;
    p_heap->size++;

    // Step 4: Heapify up.
    //
    binary_heapify_up(p_heap, p_heap->size - 1);
}

/**
 * @brief Deletes and returns the root node from the binary heap.
 *
 * @param p_heap Pointer to the binary min-heap.
 * @return maze_grid_cell_t* Pointer to the original root node.
 */
maze_grid_cell_t *
binary_heap_delete_min (binary_heap_t *p_heap)
{
    // Step 1: Save the root node to return.
    //
    binary_heap_node_t root_node = p_heap->p_array[0];

    // Step 2: Move the last node to the root and delete the last node.
    //
    p_heap->p_array[0] = p_heap->p_array[p_heap->size - 1];
    // Delete the last node by setting it to NULL.
    p_heap->p_array[p_heap->size - 1].p_maze_node = NULL;
    p_heap->p_array[p_heap->size - 1].priority    = 0;

    // Step 3: Decrease the size of the heap.
    //
    p_heap->size--;

    // Step 4: Heapify down.
    //
    binary_heapify_down(p_heap, 0);

    // Step 5: Return the saved root node.
    //
    return root_node.p_maze_node;
}

/**
 * @brief Peeks at the root node of the binary heap.
 *
 * @param p_heap Pointer to the binary min-heap.
 * @return binary_heap_node_t Root node of the binary heap.
 */
binary_heap_node_t
binary_heap_peek (binary_heap_t *p_heap)
{
    // Step 1: Return the root node.
    //
    return p_heap->p_array[0];
}

/**
 * @brief Finds the index of a given node in the binary heap if it exists.
 * Otherwise, returns UINT16_MAX.
 *
 * @param[in] p_heap Pointer to the binary heap.
 * @param[in] p_maze_node Pointer to the node to be searched for.
 * @return uint16_t Index of the node if it exists, otherwise UINT16_MAX.
 */
uint16_t
binary_heap_get_node_idx (const binary_heap_t    *p_heap,
                          const maze_grid_cell_t *p_maze_node)
{
    // Assume that the node is in the heap.
    //
    uint16_t return_index = UINT16_MAX;

    for (uint16_t index = 0; p_heap->size > index; index++)
    {
        if (p_heap->p_array[index].p_maze_node == p_maze_node)
        {
            return_index = index;
            break;
        }
    }

    return return_index;
}

// End of pathfinding/binary_heap.c
