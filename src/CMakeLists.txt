
cmake_minimum_required(VERSION 3.12)

include(FreeRTOS_Kernel_import.cmake)
if (PICO_CYW43_SUPPORTED) # set by PICO_BOARD=pico_w
    if (NOT TARGET pico_cyw43_arch)
        message("Skipping Pico W examples as support is not available")
    else()

        if (DEFINED ENV{WIFI_SSID} AND (NOT WIFI_SSID))
            set(WIFI_SSID $ENV{WIFI_SSID})
            message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
        endif()

        if (DEFINED ENV{WIFI_PASSWORD} AND (NOT WIFI_PASSWORD))
            set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
            message("Using WIFI_PASSWORD from environment")
        endif()

        set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for examples")
        set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for examples")

        # add_subdirectory(wifi)
        if (NOT TARGET pico_btstack_base)
            message("Skipping Pico W Bluetooth examples as support is not available")
        # else()
        #     add_subdirectory(bt)
        endif()
    endif()
endif()

set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for examples")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for examples")


if ("${WIFI_SSID}" STREQUAL "")
    message("Skipping some Pico W examples as WIFI_SSID is not defined")
elseif ("${WIFI_PASSWORD}" STREQUAL "")
    message("Skipping some Pico W examples as WIFI_PASSWORD is not defined")
else()

endif()

add_subdirectory(drivers)
add_subdirectory(pathfinding)

if (EXISTS ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c)
    add_executable(car_project_nosys
        main.c
        # ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
        )
    target_compile_definitions(car_project_nosys PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
    target_include_directories(car_project_nosys PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        # ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
        ${CMAKE_CURRENT_SOURCE_DIR}/pathfinding
        ${CMAKE_CURRENT_SOURCE_DIR}/pid
        ${PICO_LWIP_CONTRIB_PATH}/apps/ping
        )
    target_link_directories(car_project_nosys PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
        ${CMAKE_CURRENT_SOURCE_DIR}/pathfinding
        ${CMAKE_CURRENT_SOURCE_DIR}/pid
        )

    target_link_libraries(car_project_nosys
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        pico_lwip_iperf
        FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
        pathfinding
        wifi_driver
        encoder_driver
        irline_driver
        magnetometer_driver
        motor_driver
        ultrasonic_driver
        pid_driver
        )
    # enable usb output and uart output
    pico_enable_stdio_usb(car_project_nosys 1)
    pico_enable_stdio_uart(car_project_nosys 0)

    # create map/bin/hex file etc.
    pico_add_extra_outputs(car_project_nosys)

    # add url via pico_set_program_url
    example_auto_set_url(car_project_nosys)

    add_executable(car_project
        main.c
        ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
        )
    target_compile_definitions(car_project PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
        LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
        PING_USE_SOCKETS=1
        )
    target_include_directories(car_project PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${PICO_LWIP_CONTRIB_PATH}/apps/ping
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
        ${CMAKE_CURRENT_SOURCE_DIR}/pathfinding
        ${CMAKE_CURRENT_SOURCE_DIR}/pid
        )
    target_link_directories(car_project_nosys PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
        ${CMAKE_CURRENT_SOURCE_DIR}/pathfinding
        ${CMAKE_CURRENT_SOURCE_DIR}/pid
        )
    target_link_libraries(car_project
        hardware_adc
        pico_cyw43_arch_lwip_sys_freertos
        pico_stdlib
        pico_lwip_iperf
        FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
        pathfinding
        wifi_driver
        encoder_driver
        irline_driver
        magnetometer_driver
        motor_driver
        ultrasonic_driver
        pid_driver
        )
    pico_enable_stdio_usb(car_project 1)
    pico_enable_stdio_uart(car_project 0)
    pico_add_extra_outputs(car_project)

endif()