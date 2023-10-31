add_executable(tests
    tests.c
    )

target_link_directories(tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../main
    ${CMAKE_CURRENT_LIST_DIR}/../main/pathfinding
    )

target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../main
    )

target_link_libraries(tests PRIVATE
    pathfinding
    )