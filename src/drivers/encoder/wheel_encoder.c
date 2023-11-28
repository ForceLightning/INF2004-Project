/**
 * @file wheel_encoder.c
 * @author Bryan Seah
 * @brief Driver code for the wheel encoder.
 * @version 0.1
 * @date 2023-11-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include "encoder/wheel_encoder.h"

/**
 * @brief Get the time difference in ms
 *
 * @param[in] current_time Current time in us
 * @param[in] prev_time Previous time in us
 * @return float Time difference in ms
 */
float
wheel_enc_get_time_diff (uint64_t current_time, uint64_t prev_time)
{
    return (current_time - prev_time)
           / WHEEL_ENC_SEC_TO_MSEC; // Conversion from uint64_t to float is safe
                                    // because the maximum value of uint64_t is
                                    // greater than the maximum value of float.
}

/**
 * @brief Get the speed in either pulses/second or mm/second
 *
 * @param[in] time_elapsed Time elapsed in ms
 * @param[in] is_pulse True if speed is in pulses/second, false if speed is in
 * mm/second
 * @return float Speed in either pulses/second or mm/second
 */
float
wheel_enc_get_speed (float time_elapsed, bool is_pulse)
{
    float speed = 0.0f; // Initialize speed to 0.

    if (is_pulse)
    {
        speed = WHEEL_ENC_SEC_TO_MSEC / time_elapsed;
    }
    else
    {
        speed
            = (WHEEL_ENC_SEC_TO_MSEC / time_elapsed) * WHEEL_ENC_DIST_PER_PULSE;
    }

    return speed;
}
// End of file wheel_encoder.c.
