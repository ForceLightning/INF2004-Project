/**
 * @file wheel_encoder.h
 * @author Bryan Seah
 * @brief Header file for the wheel encoder drivers.
 * @version 0.1
 * @date 2023-11-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WHEEL_ENCODER_H // Include guard.
#define WHEEL_ENCODER_H
#include "pico/stdlib.h"

// Definitions.
// ----------------------------------------------------------------------------
//
#define WHEEL_ENC_CYCLE_PULSE \
    20 // Number of encoder pulses per 1 full rotation.
#define WHEEL_ENC_DIST_PER_PULSE \
    (204.203f / 20.0f) // Distance travelled per tick in mm.
#define WHEEL_ENC_SEC_TO_MSEC \
    1000.0f // Conversion from seconds to milliseconds.

// Type definitions.
// ----------------------------------------------------------------------------
//

/**
 * @typedef wheel_encoder_t
 * @brief Struct to store global encoder data.
 *
 */
typedef struct wheel_encoder
{
    uint64_t prev_time;          ///< Time of previous interrupt.
    uint     pulse_count;        ///< Number of pulses since last reset.
    float    distance_traversed; ///< Distance traversed since last reset.
} wheel_encoder_t;

// Function prototypes
//
float wheel_enc_get_time_diff(uint64_t current_time, uint64_t prev_time);
float wheel_enc_get_speed(float time_elapsed, bool is_pulse);

#endif // WHEEL_ENCODER_H

// End of file driver/encoder/wheel_encoder.h.