target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/a_star.h
    ${CMAKE_CURRENT_LIST_DIR}/binary_heap.h
    ${CMAKE_CURRENT_LIST_DIR}/ir_sensor.h
    ${CMAKE_CURRENT_LIST_DIR}/magnetometer.h
    ${CMAKE_CURRENT_LIST_DIR}/maze.h
    ${CMAKE_CURRENT_LIST_DIR}/motor_control.h
    ${CMAKE_CURRENT_LIST_DIR}/ultrasonic.h
    ${CMAKE_CURRENT_LIST_DIR}/wheel_encoder.h
)

target_sources(${PROJECT_NAME} PRIVATE
    ${HEADER_FILES}
)