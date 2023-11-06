set(ctests
    pathfinding
    pid
    floodfill
    )

set(pathfinding_parts
    1 2 3 4 5 6 7 8 9 10 11
    )

foreach(ctest ${ctests})
    if(NOT DEFINED "${ctest}_parts")
        set(${ctest}_parts "1")
    endif()
    message(STATUS "Detected test ${ctest} with parts ${${ctest}_parts}")
endforeach()

foreach(ctest ${ctests})
    set (ctestsrc ${ctestsrc} ${ctest}_tests.c)
    message(STATUS "Adding test ${ctest}_tests.c")
endforeach()

message(STATUS "Source files: ${ctestsrc}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

create_test_sourcelist(srclist test_runner.c ${ctestsrc})
add_executable(test_runner ${srclist} project_test.c)

target_link_directories(test_runner PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../main
    ${CMAKE_CURRENT_LIST_DIR}/../main/pathfinding
    )

target_include_directories(test_runner PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../main
    )

target_link_libraries(test_runner PRIVATE
    pathfinding
    )

foreach(ctest ${ctests})
    foreach(part ${${ctest}_parts})
        message(STATUS "Adding test test_${ctest}_${part}")
        add_test(test_${ctest}_${part}
            ${TEST_PATH}/test_runner ${ctest}_tests ${part}
            )
        set_tests_properties(test_${ctest}_${part} PROPERTIES
            FAIL_REGULAR_EXPRESSION "ERROR;FAIL;Test failed"
            )
    endforeach()
endforeach()