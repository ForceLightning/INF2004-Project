
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

add_executable(wifi_interrupt
        main.c
        wifi.c
        )
target_compile_definitions(wifi_interrupt PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(wifi_interrupt PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    # ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../../ # for common lwipopts.h and FreeRTOSConfig.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../ # for common lwipopts.h and FreeRTOSConfig.h
    )
target_link_libraries(wifi_interrupt
    pico_cyw43_arch_lwip_threadsafe_background
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4
    )

pico_add_extra_outputs(wifi_interrupt)
pico_enable_stdio_usb(wifi_interrupt 1)

add_executable(wifi_poll
        main.c        
        wifi.c  
        )
target_compile_definitions(wifi_poll PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(wifi_poll PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    # ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../../ # for common lwipopts.h and FreeRTOSConfig.h
    ${CMAKE_CURRENT_LIST_DIR}/../../../ # for common lwipopts.h and FreeRTOSConfig.h
        )
target_link_libraries(wifi_poll
    pico_cyw43_arch_lwip_threadsafe_background
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4
    )
pico_add_extra_outputs(wifi_poll)
pico_enable_stdio_usb(wifi_poll 1)


add_library(wifi_driver INTERFACE
    )

target_sources(wifi_driver INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/wifi.c
    )

target_link_libraries(wifi_driver INTERFACE
    pico_stdlib
    pico_cyw43_arch_lwip_sys_freertos
    pico_lwip_freertos
    pico_lwip_iperf
    )

target_link_directories(wifi_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(wifi_driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../ # for common lwipopts.h and FreeRTOSConfig.h
    )