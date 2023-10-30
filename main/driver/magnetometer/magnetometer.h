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

#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

// Definitions.
//
#define MAGNETOMETER_ADDR \
    0x1E // Default I2C address for the LSM303DLHC magnetometer.
#define ACCELEROMETER_ADDR \
    0x19 // Default I2C address for the LSM303DLHC accelerometer.
#define I2C_SDA_PIN  0
#define I2C_SCL_PIN  1
#define I2C_BAUDRATE 1000000 // 1 MHz baudrate for I2C communication.

// Function prototypes
void init_magnetometer();
void read_magnetometer_data();

#endif // MAGNETOMETER_H

// End of file driver/magnetometer/magnetometer.h.
