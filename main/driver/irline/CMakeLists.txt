
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