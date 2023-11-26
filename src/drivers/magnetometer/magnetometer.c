/**
 * @file magnetometer.c
 *
 * @author Jurgen Tan
 *
 * @brief This file contains functions for initializing and reading data from a
 * magnetometer and accelerometer sensors using I2C communication. The module
 * used is the LSM303DLHC.
 *
 * @version 0.2
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "magnetometer.h"

int16_t g_bias_x = 0; // Bias for x-axis acceleration.
int16_t g_bias_y = 0; // Bias for y-axis acceleration.
int16_t g_bias_z = 0; // Bias for z-axis acceleration.

float *gp_true_heading    = 0; // True heading in radians.
float *gp_current_bearing = 0; // Current bearing in radians.
float *gp_min_bearing     = 0; // Minimum bearing in radians.
float *gp_max_bearing     = 0; // Maximum bearing in radians.

// Private function prototypes.
// -----------------------------------------------------------------------------
//
static void magneto_write_register(uint8_t address, uint8_t reg, uint8_t value);
static uint8_t magneto_read_register(uint8_t address, uint8_t reg);
static void    magneto_config_accel(void);
static void    magneto_read_accel(int16_t *p_x, int16_t *p_y, int16_t *p_z);
static void    magneto_calibrate_accel(void);
static void    magneto_calculate_accel(int16_t x_accel,
                                       int16_t y_accel,
                                       int16_t z_accel);
static void    magneto_init_i2c(void);
static void    magneto_configure(void);

// Public functions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Reads magnetometer data from the magnetometer sensor and outputs the
 * values into the global variables.
 *
 */
void
magneto_read_data (void)
{

    for (;;) // Infinite loop.
    {
        int16_t x_acc; // x-axis acceleration
        int16_t y_acc; // y-axis acceleration
        int16_t z_acc; // z-axis acceleration
        magneto_read_accel(&x_acc, &y_acc, &z_acc);
        magneto_calculate_accel(x_acc, y_acc, z_acc);

        uint8_t reg = 0x03; // Register to read from.
        uint8_t data[6];
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, &reg, 1, true);
        i2c_read_blocking(i2c0, MAGNETOMETER_ADDR, data, 6, false);

        int16_t x_mag           = (int16_t)((data[0] << 8) | data[1]);
        int16_t y_mag           = (int16_t)((data[4] << 8) | data[5]);
        float   heading_radians = (float)atan2(
            y_mag,
            x_mag); // Double to float is safe, loss of precision is acceptable.

        *gp_current_bearing = heading_radians;

        // Set true heading and min/max bearing if not set.
        //
        if (!*gp_true_heading)
        {
            *gp_true_heading = heading_radians;
            *gp_min_bearing  = heading_radians - BEARING_OFFSET;
            *gp_max_bearing  = heading_radians + BEARING_OFFSET;
        }

        printf("Compass Heading: %f\n", heading_radians);
        printf("True North: %f\n", *gp_true_heading);
        printf("Curr Bearing: %f\n", *gp_true_heading);
        sleep_ms(1000);
    }
}

/**
 * @brief Checks if the current bearing is invalid if it is out of bounds.
 *
 * @return true Bearing is invalid.
 * @return false Bearing is valid.
 */
bool
magneto_is_bearing_invalid ()
{
    return (*gp_current_bearing < *gp_min_bearing
            || *gp_current_bearing > *gp_max_bearing);
}

/**
 * @brief Gets the true bearing of the magnetometer sensor.
 *
 * @return float True bearing in radians.
 */
float
magneto_get_true_bearing (void)
{
    return *gp_true_heading;
}

/**
 * @brief Gets the current bearing of the magnetometer sensor.
 *
 * @return float Current bearing in radians.
 */
float
magneto_get_curr_bearing (void)
{
    return *gp_current_bearing;
}

/**
 * @brief Initializes the magnetometer by initializing the USB, I2C, and setting
 * the GPIO pins.
 */
void
magneto_init (void)
{
    magneto_init_i2c();
    magneto_config_accel();
    magneto_calibrate_accel();
    magneto_configure();
}

// Private functions.
// -----------------------------------------------------------------------------
//

/**
 * @brief Writes a value to a register on the magnetometer sensor.
 *
 * @param address I2C address of the magnetometer sensor.
 * @param reg Register to write to.
 * @param value Value to write to the register.
 */
void
magneto_write_register (uint8_t address, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = { reg, value };
    i2c_write_blocking(i2c0, address, data, 2, true);
}

/**
 * @brief Reads a value from a register on the magnetometer sensor.
 *
 * @param[in] address I2C address of the magnetometer sensor.
 * @param[in] reg Register to read from.
 * @return uint8_t Value read from the register.
 */
uint8_t
magneto_read_register (uint8_t address, uint8_t reg)
{
    uint8_t data;
    i2c_write_blocking(i2c0, address, &reg, 1, true);
    i2c_read_blocking(i2c0, address, &data, 1, false);
    return data;
}

/**
 * @brief Configures the accelerometer sensor by setting the control register.
 *
 */
void
magneto_config_accel (void)
{
    magneto_write_register(ACCELEROMETER_ADDR, CTRL_REG1_A, 0x47);
}

/**
 * @brief Reads the acceleration data from the accelerometer sensor.
 *
 * @param[out] p_x Pointer to the x-axis acceleration.
 * @param[out] p_y Pointer to the y-axis acceleration.
 * @param[out] p_z Pointer to the z-axis acceleration.
 */
void
magneto_read_accel (int16_t *p_x, int16_t *p_y, int16_t *p_z)
{
    *p_x = (int16_t)((magneto_read_register(ACCELEROMETER_ADDR, OUT_X_H_A) << 8)
                     | magneto_read_register(ACCELEROMETER_ADDR, OUT_X_L_A))
           - g_bias_x;
    *p_y = (int16_t)((magneto_read_register(ACCELEROMETER_ADDR, OUT_Y_H_A) << 8)
                     | magneto_read_register(ACCELEROMETER_ADDR, OUT_Y_L_A))
           - g_bias_y;
    *p_z = (int16_t)((magneto_read_register(ACCELEROMETER_ADDR, OUT_Z_H_A) << 8)
                     | magneto_read_register(ACCELEROMETER_ADDR, OUT_Z_L_A))
           - g_bias_z;
}

/**
 * @brief Calibrates the accelerometer sensor by taking 100 samples of the
 * acceleration data and calculating the average of the samples. The average is
 * then used as the bias for the acceleration data.
 *
 */
void
magneto_calibrate_accel (void)
{
    const int num_samples = 100;
    int16_t   x_accum     = 0;
    int16_t   y_accum     = 0;
    int16_t   z_accum     = 0;

    for (int i = 0; i < num_samples; i++)
    {
        int16_t x_accel;
        int16_t y_accel;
        int16_t z_accel;
        magneto_read_accel(&x_accel, &y_accel, &z_accel);
        x_accum += x_accel;
        y_accum += y_accel;
        z_accum += z_accel;
        sleep_ms(10);
    }

    g_bias_x = x_accum / num_samples;
    g_bias_y = y_accum / num_samples;
    g_bias_z = z_accum / num_samples;
}

/**
 * @brief Caclulates the overall acceleration from the acceleration data of the
 * x, y, and z axes, compared to the gravitational acceleration constant.
 *
 * @param x_accel Acceleration data for the x-axis.
 * @param y_accel Acceleration data for the y-axis.
 * @param z_accel Acceleration data for the z-axis.
 */
void
magneto_calculate_accel (int16_t x_accel, int16_t y_accel, int16_t z_accel)
{
    double acc_x = (double)x_accel * (GRAVITY_CONSTANT_FACTOR);
    double acc_y = (double)y_accel * (GRAVITY_CONSTANT_FACTOR);
    double acc_z = (double)z_accel * (GRAVITY_CONSTANT_FACTOR);

    double acceleration = sqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);
    printf("Overall Acceleration: %.2f m/s^2\n", acceleration);
}

/**
 * @brief Initializes the I2C communication by setting the baudrate and setting
 * the GPIO pins.
 *
 */
void
magneto_init_i2c (void)
{
    i2c_init(i2c0, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

/**
 * @brief Configures the magnetometer sensor by writing to the magnetometer I2C
 * address and register.
 *
 */
void
magneto_configure (void)
{
    magneto_write_register(MAGNETOMETER_ADDR, MR_REG_M, CRA_REG_M);
}

// End of file driver/magnetometer/magnetometer.c.
