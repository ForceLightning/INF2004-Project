add_library(pathfinding INTERFACE
)

target_sources(pathfinding INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/a_star.c
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maze.c
)

target_link_libraries(pathfinding INTERFACE
    pico_stdlib
)

target_include_directories(pathfinding INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)