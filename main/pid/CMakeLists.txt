
cmake_minimum_required(VERSION 3.12)

if (DEFINED ENV{WIFI_SSID} AND (NOT WIFI_SSID))
    set(WIFI_SSID $ENV{WIFI_SSID})
    message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
endif()

if (DEFINED ENV{WIFI_PASSWORD} AND (NOT WIFI_PASSWORD))
    set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
    message("Using WIFI_PASSWORD from environment ('${WIFI_PASSWORD}')")
endif()

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password")

add_executable(pid
    main.c pid.c
    )

target_sources(pid PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/pid.c
    )

target_link_libraries(pid PRIVATE
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    hardware_pwm
    driver
    pathfinding
    )

pico_add_extra_outputs(pid)

pico_enable_stdio_usb(pid 1)
pico_enable_stdio_uart(pid 0)

add_library(pid_driver INTERFACE)

target_sources(pid_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/pid.c
    )

target_link_libraries(pid_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_pwm
    driver
    pathfinding
    )

target_include_directories(pid_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    )
