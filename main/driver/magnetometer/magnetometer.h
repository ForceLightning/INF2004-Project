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

#define CTRL_REG1_A 0x20

#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D

#define CRA_REG_M 0x00
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Z_H_M 0x05
#define OUT_Z_L_M 0x06
#define OUT_Y_H_M 0x07
#define OUT_Y_L_M 0x08

#define BEARING_OFFSET 2.0f

typedef struct bearing_data
{
    bool is_initialised;
    float true_heading;
    float current_bearing;
    float min_bearing;
    float max_bearing;
} bearing_data_t;

void init_bearing_data(bearing_data_t *p_bearing_data);


// Function prototypes
void init_magnetometer();
void readMagnetometerData(bearing_data_t *p_bearing_data);
// void reset_true_north();
uint checkBearingOutOfRange(bearing_data_t *p_bearing_data);
float getTrueBearing(bearing_data_t *p_bearing_data);
float getCurrentBearing(bearing_data_t *p_bearing_data);
#endif // MAGNETOMETER_H

// End of file driver/magnetometer/magnetometer.h.
