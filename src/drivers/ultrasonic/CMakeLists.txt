
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

add_executable(ultrasonic
    main.c
    )

target_link_libraries(ultrasonic
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    )

target_include_directories(ultrasonic PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )

pico_add_extra_outputs(ultrasonic)

pico_enable_stdio_usb(ultrasonic 1)
pico_enable_stdio_uart(ultrasonic 0)

add_library(ultrasonic_driver INTERFACE
    )

target_sources(ultrasonic_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic.c
    )

target_link_libraries(ultrasonic_driver INTERFACE
    pico_stdlib
    hardware_gpio
    )

target_link_directories(ultrasonic_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(ultrasonic_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )