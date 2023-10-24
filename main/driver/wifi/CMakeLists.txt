
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

add_executable(wifi_nosys
    main.c
    )

target_compile_definitions(wifi_nosys PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    )

target_include_directories(wifi_nosys PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    # ${CMAKE_CURRENT_LIST_DIR}/../
    ${CMAKE_CURRENT_LIST_DIR}/../../ # for common lwipopts.h and FreeRTOSConfig.h
    # ${CMAKE_CURRENT_LIST_DIR}/../../../ # for common lwipopts.h and FreeRTOSConfig.h
    ${PICO_LWIP_CONTRIB_PATH}/apps/ping
    )

target_link_libraries(wifi_nosys
    pico_cyw43_arch_lwip_threadsafe_background
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4
    )

pico_add_extra_outputs(wifi_nosys)

pico_enable_stdio_usb(wifi_nosys 1)
pico_enable_stdio_uart(wifi_nosys 0)


add_executable(wifi_sys
    main.c
    )

target_compile_definitions(wifi_sys PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    NO_SYS=0
    LWIP_SOCKET=1
    PING_USE_SOCKETS=1
    )

target_include_directories(wifi_sys PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../ # for common lwipopts.h and FreeRTOSConfig.h
    # ${CMAKE_CURRENT_LIST_DIR}/../../../ # for common lwipopts.h and FreeRTOSConfig.h
    ${PICO_LWIP_CONTRIB_PATH}/apps/ping
    )

target_link_libraries(wifi_sys
    pico_cyw43_arch_lwip_sys_freertos
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4
    )

pico_add_extra_outputs(wifi_sys)

pico_enable_stdio_usb(wifi_sys 1)
pico_enable_stdio_uart(wifi_sys 0)