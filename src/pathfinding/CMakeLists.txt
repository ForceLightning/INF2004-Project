add_library(pathfinding INTERFACE
)

target_sources(pathfinding INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/a_star.c
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maze.c
    ${CMAKE_CURRENT_SOURCE_DIR}/floodfill.c
    ${CMAKE_CURRENT_SOURCE_DIR}/dfs.c
)

target_include_directories(pathfinding INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)