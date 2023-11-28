
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

add_executable(encoder
    main.c
    wheel_encoder.c
    )

target_link_libraries(encoder PRIVATE
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    hardware_pwm
    )

target_include_directories(encoder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_directories(encoder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

pico_add_extra_outputs(encoder)

pico_enable_stdio_usb(encoder 1)
pico_enable_stdio_uart(encoder 0)

add_library(encoder_driver INTERFACE
    )

target_sources(encoder_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/wheel_encoder.c
    )

target_link_libraries(encoder_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_pwm
    )

target_link_directories(encoder_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(encoder_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    )