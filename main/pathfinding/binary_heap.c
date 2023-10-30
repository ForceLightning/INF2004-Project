#include <stdint.h>
#include "binary_heap.h"
#include "maze.h"

// Private function prototypes.
//
static void heapify_up(binary_heap_t *p_heap, uint16_t index);
static void heapify_down(binary_heap_t *p_heap, uint16_t index);

/**
 * @brief Moves the node at the given index down the binary heap to maintain the
 * heap property. This in effect raises the node to its correct position.
 *
 * @param p_heap Pointer to the binary heap.
 * @param index Index of the node to be moved down.
 */
static void
heapify_up (binary_heap_t *p_heap, uint16_t index)
{
    // @NOTE: These early returns are not necessary, but they make the code
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
        heap_node_t temp_node         = p_heap->p_array[parent_index];
        p_heap->p_array[parent_index] = p_heap->p_array[index];
        p_heap->p_array[index]        = temp_node;

        // Step 5: Heapify up the parent node.
        // Update the index to the parent's index and recalculate the node's new
        // parent's index.
        //
        index        = parent_index;
        parent_index = (index - 1) / 2;
    }
}

/**
 * @brief Moves the node at the given index up the binary heap to maintain the
 * heap property. This in effect lowers the node to its correct position.
 *
 * @param p_heap Pointer to the binary heap.
 * @param index Index of the node to be moved up.
 */
static void
heapify_down (binary_heap_t *p_heap, uint16_t index)
{
    // Step 1.1: Declare variables to store the indices of the left and right,
    // as well as the smallest child.
    //
    uint16_t left_index;
    uint16_t right_index;
    uint16_t smallest_child_index;

    for (;;)
    {
        // Step 1.2: Calculate the left and right child indices. In a binary
        // heap, the indices are known to be 2 * index + 1..=2 for the left and
        // right respectively.
        //
        left_index           = 2 * index + 1;
        right_index          = 2 * index + 2;
        smallest_child_index = index;

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
        heap_node_t temp_node  = p_heap->p_array[index];
        p_heap->p_array[index] = p_heap->p_array[smallest_child_index];
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
 * @param p_heap Pointer to the binary heap.
 * @param p_maze_node Pointer to the grid cell node to be inserted.
 * @param priority Priority of the node to be inserted.
 */
void
insert (binary_heap_t *p_heap, grid_cell_t *p_maze_node, uint16_t priority)
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
    heap_node_t new_node;
    new_node.priority    = priority;
    new_node.p_maze_node = p_maze_node;

    // Step 3: Insert it at the end of the array.
    //
    p_heap->p_array[p_heap->size] = new_node;
    p_heap->size++;

    // Step 4: Heapify up.
    //
    heapify_up(p_heap, p_heap->size - 1);
}

/**
 * @brief Deletes and returns the root node from the binary heap.
 *
 * @param p_heap Pointer to the binary heap.
 * @return grid_cell_t* Pointer to the original root node.
 */
grid_cell_t *
delete_min (binary_heap_t *p_heap)
{
    // Step 1: Save the root node to return.
    //
    heap_node_t root_node = p_heap->p_array[0];

    // Step 2: Move the last node to the root.
    //
    p_heap->p_array[0] = p_heap->p_array[p_heap->size - 1];

    // Step 3: Decrease the size of the heap.
    //
    p_heap->size--;

    // Step 4: Heapify down.
    //
    heapify_down(p_heap, 0);

    // Step 5: Return the saved root node.
    //
    return root_node.p_maze_node;
}

/**
 * @brief Peeks at the root's value without removal.
 *
 * @param p_heap Pointer to the binary heap.
 * @return heap_node_t The root's value.
 */
heap_node_t
peek (binary_heap_t *p_heap)
{
    // Step 1: Return the root node.
    //
    return p_heap->p_array[0];
}