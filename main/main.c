#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "drivers/ir_sensor/ir_sensor.h"
#include "drivers/magnetometer/magnetometer.h"
#include "drivers/motor_controller/motor_control.h"
#include "drivers/ultrasonic_sensor/ultrasonic.h"
#include "drivers/wheel_encoder/wheel_encoder.h"

#include "pathfinding/a_star.h"

// TODO: Setup FreeRTOS.

int
main (void)
{
    return 0;
}