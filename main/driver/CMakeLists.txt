add_library(driver INTERFACE
)

target_sources(driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/encoder/wheel_encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/irline/ir_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/irline/barcode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magnetometer/magnetometer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/motor/motor_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic/ultrasonic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi/wifi.c
)

target_link_libraries(driver INTERFACE
    pico_lwip_iperf
    pico_stdlib
    hardware_adc
    hardware_gpio
    hardware_pwm
    hardware_i2c
)

target_include_directories(driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_subdirectory(encoder)
add_subdirectory(irline)
add_subdirectory(magnetometer)
add_subdirectory(motor)
add_subdirectory(ultrasonic)
add_subdirectory(wifi)