
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

add_executable(irline
    main.c
    ir_sensor.c
    barcode.c
    )

target_link_libraries(irline
    pico_stdlib
    pico_cyw43_arch_none
    hardware_gpio
    hardware_adc
    )

pico_add_extra_outputs(irline)

pico_enable_stdio_usb(irline 1)
pico_enable_stdio_uart(irline 0)

add_library(irline_driver INTERFACE
    )

target_sources(irline_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/barcode.c
    )

target_link_libraries(irline_driver INTERFACE
    pico_stdlib
    hardware_gpio
    hardware_adc
    )

target_link_directories(irline_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(irline_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )