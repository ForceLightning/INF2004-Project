add_library(car_library
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_sensor/ir_sensor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/magnetometer/magnetometer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/motor_controller/motor_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic_sensor/ultrasonic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/wheel_encoder/wheel_encoder.c
)

target_include_directories(car_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

export(TARGETS car_library FILE car_library.cmake)