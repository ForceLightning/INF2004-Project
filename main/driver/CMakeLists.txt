add_library(driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/irline/ir_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magnetometer/magnetometer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/motor/motor_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic/ultrasonic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/encoder/wheel_encoder.c
)

target_link_libraries(driver INTERFACE
    pico_stdlib
    hardware_adc
    hardware_gpio
)

target_include_directories(driver INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)