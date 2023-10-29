#ifndef WHEEL_ENCODER_H
#define WHEEL_ENCODER_H
#include "pico/stdlib.h"

// Function prototypes
float get_time_diff(uint64_t current_time, uint64_t prev_time);
float get_speed(float time_elapsed, uint is_pulse);

#endif // WHEEL_ENCODER_H