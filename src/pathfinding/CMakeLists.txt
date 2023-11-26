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

add_executable(
    pathfinding_pico_tests
    main.c
)

target_include_directories(pathfinding_pico_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(pathfinding_pico_tests PRIVATE
    pico_stdlib
    pico_cyw43_arch_none
    pathfinding
)

pico_add_extra_outputs(pathfinding_pico_tests)

pico_enable_stdio_usb(pathfinding_pico_tests 1)
pico_enable_stdio_uart(pathfinding_pico_tests 0)