cmake_minimum_required(VERSION 3.12)

if (DEFINED ENV{TEST_BUILD})
    project(car_project_tests C CXX ASM)
    message(STATUS "Testing build")
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_CXX_STANDARD 17)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(main/pathfinding)
else()
    include(pico_sdk_import.cmake)

    include(pico_extras_import_optional.cmake)

    project(car_project C CXX ASM)
    set(PICO_BOARD pico_w)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 17)

    if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
        message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
    endif()

    if (DEFINED ENV{PICO_EXAMPLES_PATH} AND (NOT PICO_EXAMPLES_PATH))
        set(PICO_EXAMPLES_PATH $ENV{PICO_EXAMPLES_PATH})
    else()
        set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR}/pico-examples)
    endif()

    # Initialize the SDK
    pico_sdk_init()

    include(example_auto_set_url.cmake)

    add_compile_options(-Wall
            -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
            -Wno-unused-function # we have some for the docs that aren't called
            )
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-maybe-uninitialized)
    endif()

    # Directories
    add_subdirectory(main)

    target_include_directories(car_project PRIVATE
        ${PICO_SDK_PATH}/include
        ${FREERTOS_KERNEL_PATH}/include
    )
endif()
