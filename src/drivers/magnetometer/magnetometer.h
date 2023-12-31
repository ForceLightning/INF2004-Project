/**
 * @file magnetometer.h
 * @author Jurgen Tan
 * @brief Header file for the magnetometer driver.
 * @version 0.1
 * @date 2023-10-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MAGNETOMETER_H // Include guard.
#define MAGNETOMETER_H

// Definitions.
// -----------------------------------------------------------------------------
//

/**
 * @defgroup magnetometer_constants Magnetometer Constants
 * @brief Constants for the magnetometer driver.
 * @{
 */

/** @brief Default I2C address for the LSM303DLHC magnetometer. */
#define MAGNETO_METER_ADDR 0x1E
/** @brief Default I2C address for the LSM303DLHC accelerometer. */
#define MAGNETO_ACCEL_ADDR 0x19
/** @brief SDA pin for I2C communication. */
#define MAGNETO_I2C_SDA_PIN 0
/** @brief SCL pin for I2C communication. */
#define MAGNETO_I2C_SCL_PIN 1
/** @brief 1 MHz baudrate for I2C communication. */
#define MAGNETO_I2C_BAUDRATE 1000000

/** @brief Register for the accelerometer control. */
#define MAGNETO_CTRL_REG1_A 0x20

/** @brief Register for the x-axis acceleration (low). */
#define MAGNETO_OUT_X_L_A 0x28
/** @brief Register for the x-axis acceleration (high). */
#define MAGNETO_OUT_X_H_A 0x29
/** @brief Register for the y-axis acceleration (low). */
#define MAGNETO_OUT_Y_L_A 0x2A
/** @brief Register for the y-axis acceleration (high). */
#define MAGNETO_OUT_Y_H_A 0x2B
/** @brief Register for the z-axis acceleration (low). */
#define MAGNETO_OUT_Z_L_A 0x2C
/** @brief Register for the z-axis acceleration (high). */
#define MAGNETO_OUT_Z_H_A 0x2D

/** @brief Register for the magnetometer control. */
#define MAGNETO_CRA_REG_M 0x00
/** @brief Register for the magnetometer mode selection. */
#define MAGNETO_MR_REG_M 0x02
/** @brief Register for the x-axis magnetometer (high). */
#define MAGNETO_OUT_X_H_M 0x03
/** @brief Register for the x-axis magnetometer (low). */
#define MAGNETO_OUT_X_L_M 0x04
/** @brief Register for the z-axis magnetometer (high). */
#define MAGNETO_OUT_Z_H_M 0x05
/** @brief Register for the z-axis magnetometer (low). */
#define MAGNETO_OUT_Z_L_M 0x06
/** @brief Register for the y-axis magnetometer (high). */
#define MAGNETO_OUT_Y_H_M 0x07
/** @brief Register for the y-axis magnetometer (low). */
#define MAGNETO_OUT_Y_L_M 0x08

/** @brief Offset for bearing. */
#define MAGNETO_BEARING_OFFSET 1.0f
/** @brief 1 g = 16384 LSB */
#define MAGNETO_GRAVITY_CONSTANT_F 9.80665 / 16384.0

/**
  @} */ // End of magnetometer_constants group.

// Public Function prototypes.
//
void  magneto_init(void);
void  magneto_read_data(void);
bool  magneto_is_bearing_invalid(void);
float magneto_get_true_bearing(void);
float magneto_get_curr_bearing(void);

#endif // MAGNETOMETER_H

// End of file driver/magnetometer/magnetometer.h.
