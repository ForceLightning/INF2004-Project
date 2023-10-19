add_library(pathfinding
    ${CMAKE_CURRENT_LIST_DIR}/a_star.c
    ${CMAKE_CURRENT_LIST_DIR}/binary_heap.c
    ${CMAKE_CURRENT_LIST_DIR}/maze.c
)

target_include_directories(pathfinding PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

export(TARGETS pathfinding FILE pathfinding.cmake)