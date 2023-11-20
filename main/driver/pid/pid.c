#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include "pid.h"

/**
 * @brief Get the time difference in ms
 *
 * @param current_time Current time in us
 * @param prev_time Previous time in us
 * @return float Time difference in ms
 */
float
get_time_diff (uint64_t current_time, uint64_t prev_time)
{
    return (current_time - prev_time)
           / 1000.0f; // Conversion from uint64_t to float is safe because the
                      // maximum value of uint64_t is greater than the maximum
                      // value of float.
}

/**
 * @brief Get the speed in either pulses/second or mm/second
 *
 * @param time_elapsed Time elapsed in ms
 * @param is_pulse True if speed is in pulses/second, false if speed is in
 * mm/second
 * @return float Speed in either pulses/second or mm/second
 */
float
get_speed (float time_elapsed, bool is_pulse)
{
    float speed = 0.0f;

    if (is_pulse)
    {
        speed = 1000.0f / time_elapsed;
    }
    else
    {
        speed = (1000.0f / time_elapsed) * DISTANCE_PER_PULSE;
    }

    return speed;
}
// End of file wheel_encoder.c.
