
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

add_executable(magnetometer main.c magnetometer.c)


target_link_libraries(magnetometer
    pico_stdlib
    hardware_i2c
    pico_cyw43_arch_none
    )

pico_add_extra_outputs(magnetometer)

pico_enable_stdio_usb(magnetometer 1)
pico_enable_stdio_uart(magnetometer 0)

pico_add_extra_outputs(magnetometer)

example_auto_set_url(magnetometer)

add_library(magnetometer_driver INTERFACE
    )

target_sources(magnetometer_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/magnetometer.c
    )

target_link_libraries(magnetometer_driver INTERFACE
    pico_stdlib
    hardware_i2c
    )

target_link_directories(magnetometer_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(magnetometer_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)