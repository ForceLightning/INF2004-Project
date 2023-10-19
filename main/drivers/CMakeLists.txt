add_library(drivers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_sensor/ir_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magnetometer/magnetometer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/motor_controller/motor_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic_sensor/ultrasonic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wheel_encoder/wheel_encoder.c
)

target_link_libraries(drivers INTERFACE
    pico_stdlib
    hardware_adc
    hardware_gpio
)

target_include_directories(drivers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)