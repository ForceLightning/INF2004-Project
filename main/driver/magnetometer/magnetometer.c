/**
 * @file magnetometer.c
 *
 * @author Jurgen Tan
 *
 * @brief This file contains functions for initializing and reading data from a
 * magnetometer sensor using I2C communication. The magnetometer sensor used is
 * the LSM303DLHC. The file contains two functions: init_magnetometer() and
 * read_magnetometer_data().
 *
 * @version 0.1
 * @date 2023-10-29
 *
 * @copyright Copyright (c) 2023
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "pico/stdio_usb.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "magnetometer.h"

int16_t bias_x = 0;
int16_t bias_y = 0;
int16_t bias_z = 0;

void writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(i2c0, address, data, 2, true);
}

uint8_t readRegister(uint8_t address, uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(i2c0, address, &reg, 1, true);
    i2c_read_blocking(i2c0, address, &data, 1, false);
    return data;
}

void configureAccelerometer() {
    writeRegister(ACCELEROMETER_ADDR, CTRL_REG1_A, 0x47);
}

void readAccelerometerData(int16_t* x, int16_t* y, int16_t* z) {
    *x = (int16_t)((readRegister(ACCELEROMETER_ADDR, OUT_X_H_A) << 8) | readRegister(ACCELEROMETER_ADDR, OUT_X_L_A)) - bias_x;
    *y = (int16_t)((readRegister(ACCELEROMETER_ADDR, OUT_Y_H_A) << 8) | readRegister(ACCELEROMETER_ADDR, OUT_Y_L_A)) - bias_y;
    *z = (int16_t)((readRegister(ACCELEROMETER_ADDR, OUT_Z_H_A) << 8) | readRegister(ACCELEROMETER_ADDR, OUT_Z_L_A)) - bias_z;
}

void calibrateAccelerometer() {
    const int numSamples = 100;
    int16_t x_accum = 0;
    int16_t y_accum = 0;
    int16_t z_accum = 0;

    for (int i = 0; i < numSamples; i++) {
        int16_t x, y, z;
        readAccelerometerData(&x, &y, &z);
        x_accum += x;
        y_accum += y;
        z_accum += z;
        sleep_ms(10);
    }

    bias_x = x_accum / numSamples;
    bias_y = y_accum / numSamples;
    bias_z = z_accum / numSamples;
}

void calculateAcceleration(int16_t x, int16_t y, int16_t z) {
    double g = 9.81; // Standard gravity in m/s^2
    double acc_x = (double)x * (g / 16384.0);
    double acc_y = (double)y * (g / 16384.0);
    double acc_z = (double)z * (g / 16384.0);

    double acceleration = sqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);
    printf("Overall Acceleration: %.2f m/s^2\n", acceleration);
}


void initializeI2C() {
    i2c_init(i2c0, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

void configureMagnetometer() {
    writeRegister(MAGNETOMETER_ADDR, MR_REG_M, CRA_REG_M);
}

void readMagnetometerData() {
    for (;;) {
        int16_t x_acc, y_acc, z_acc;
        readAccelerometerData(&x_acc, &y_acc, &z_acc);
        calculateAcceleration(x_acc, y_acc, z_acc);

        uint8_t reg = 0x03;
        uint8_t data[6];
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, &reg, 1, true);
        i2c_read_blocking(i2c0, MAGNETOMETER_ADDR, data, 6, false);

        int16_t x_mag = (int16_t)((data[0] << 8) | data[1]);
        // int16_t z_mag = (int16_t)((data[2] << 8) | data[3]);
        int16_t y_mag = (int16_t)((data[4] << 8) | data[5]);
        float headingRadians = atan2(y_mag, x_mag);
        float headingDegrees = headingRadians * 180.0 / M_PI;

        if (headingDegrees < 0) {
            headingDegrees += 360.0;
        }

        printf("Compass Heading: %f\n", headingDegrees);
        sleep_ms(1000);
    }
}

/**
 * @brief Initializes the magnetometer by initializing the USB, I2C, and setting
 * the GPIO pins.
 */
void init_magnetometer(void) {
    // stdio_usb_init();
    initializeI2C();
    configureAccelerometer();
    calibrateAccelerometer();
}

// double getCompassBearing(int16_t x, int16_t y) {
//     double angle = atan2((double)y, (double)x);
//     if (angle < 0) {
//         angle += 2 * M_PI;
//     }
//     return (angle * 180.0) / M_PI;
// }


/**
 * @brief Reads magnetometer data from the magnetometer sensor and outputs the
 * data for the X, Y, and Z axes. The function continuously reads the data and
 * outputs it every second. The function uses I2C communication to communicate
 * with the magnetometer sensor. The function sets the configuration registers
 * for the magnetometer sensor before reading the data. The function also sets
 * the configuration registers for the accelerometer sensor, but does not read
 * its data.
 *
 */

// void
// read_magnetometer_data (void)
// {
    
//     for (;;)
//     {
//         int16_t x_acc, y_acc, z_acc;

//         readAccelerometerData(&x_acc, &y_acc, &z_acc);
//         calculateAcceleration(x_acc, y_acc, z_acc);
        

        

//         // Select MR register(0x02)
//         // Continuous conversion(0x00)
//         uint8_t config[] = {0x02, 0x00};
//         i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

//         // Select CRA register(0x00)
//         // Data output rate = 15Hz(0x10)
//         config[0] = 0x00;
//         config[1] = 0x10;
//         i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

//         // Select CRB register(0x01)
//         // Set gain = +/- 1.3g(0x20)
//         config[0] = 0x01;
//         config[1] = 0x20;
//         i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

//         // Read magnetometer data
//         // msb first
//         // Read xMag msb data from register(0x03)
//         uint8_t reg = 0x03;
//         uint8_t data[6];
//         i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, &reg, 1, true);
//         i2c_read_blocking(i2c0, MAGNETOMETER_ADDR, data, 6, false);

//         int16_t x_mag = (int16_t)((data[0] << 8) | data[1]);
//         int16_t z_mag = (int16_t)((data[2] << 8) | data[3]);
//         int16_t y_mag = (int16_t)((data[4] << 8) | data[5]);
        
//         float headingDegrees = getCompassBearing(x_mag, y_mag);

//         // Output magnetometer data
//         printf("Compass Heading: %f\n", headingDegrees);

//         sleep_ms(1000);
//     }
// }
// // End of file driver/magnetometer/magnetometer.c.
