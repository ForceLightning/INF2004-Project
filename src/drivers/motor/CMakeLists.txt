
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

add_executable(motor
    main.c motor_control.c
    )

target_link_libraries(motor
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    hardware_pwm
    )

target_include_directories(motor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )

pico_add_extra_outputs(motor)

pico_enable_stdio_usb(motor 1)
pico_enable_stdio_uart(motor 0)

add_library(motor_driver INTERFACE
    )

target_sources(motor_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/motor_control.c
    )

target_link_libraries(motor_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_pwm
    )

target_link_directories(motor_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(motor_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )