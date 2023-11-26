#ifndef WHEEL_ENCODER_H
#define WHEEL_ENCODER_H
#include "pico/stdlib.h"

// Definitions.
// 
#define CYCLE_PULSE 20 // Number of encoder pulses per 1 full rotation.
#define DISTANCE_PER_PULSE \
    (204.203f / 20.0f) // Distance travelled per tick in mm.

// Function prototypes
//
float get_time_diff(uint64_t current_time, uint64_t prev_time);
float get_speed(float time_elapsed, bool is_pulse);

#endif
// WHEEL_ENCODER_H
