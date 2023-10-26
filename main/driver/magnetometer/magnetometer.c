#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "magnetometer.h"

#define MAGNETOMETER_ADDR \
    0x1E // Default I2C address for the LSM303DLHC magnetometer
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1
#define ACCELEROMETER_ADDR \
    0x19 // Default I2C address for the LSM303DLHC accelerometer

void
init_magnetometer ()
{
    stdio_usb_init();

    // Initialize I2C
    i2c_init(i2c0, 1000000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

void
read_magnetometer_data ()
{
    while (1)
    {
        uint8_t config[] = { 0x20, 0x27 };
        i2c_write_blocking(i2c0, ACCELEROMETER_ADDR, config, 2, true);

        // Select control register4(0x23)
        // Full scale +/- 2g, continuous update(0x00)
        config[0] = 0x23;
        config[1] = 0x00;
        i2c_write_blocking(i2c0, ACCELEROMETER_ADDR, config, 2, true);

        // Read 6 bytes of data
        uint8_t data[6];

        // Read accelerometer data
        // lsb first
        // Read xAccl lsb data from register(0x28)
        uint8_t reg = 0x28;
        i2c_write_blocking(i2c0, ACCELEROMETER_ADDR, &reg, 1, true);
        i2c_read_blocking(i2c0, ACCELEROMETER_ADDR, data, 6, false);

        // int xAccl = (int16_t)((data[1] << 8) | data[0]);
        // int yAccl = (int16_t)((data[3] << 8) | data[2]);
        // int zAccl = (int16_t)((data[5] << 8) | data[4]);

        // Output accelerometer data
        // printf("Acceleration in X-Axis: %d\n", xAccl);
        // printf("Acceleration in Y-Axis: %d\n", yAccl);
        // printf("Acceleration in Z-Axis: %d\n", zAccl);

        // Select MR register(0x02)
        // Continuous conversion(0x00)
        config[0] = 0x02;
        config[1] = 0x00;
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

        // Select CRA register(0x00)
        // Data output rate = 15Hz(0x10)
        config[0] = 0x00;
        config[1] = 0x10;
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

        // Select CRB register(0x01)
        // Set gain = +/- 1.3g(0x20)
        config[0] = 0x01;
        config[1] = 0x20;
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, config, 2, true);

        // Read magnetometer data
        // msb first
        // Read xMag msb data from register(0x03)
        reg = 0x03;
        i2c_write_blocking(i2c0, MAGNETOMETER_ADDR, &reg, 1, true);
        i2c_read_blocking(i2c0, MAGNETOMETER_ADDR, data, 6, false);

        int xMag = (int16_t)((data[0] << 8) | data[1]);
        int yMag = (int16_t)((data[2] << 8) | data[3]);
        int zMag = (int16_t)((data[4] << 8) | data[5]);

        // Output magnetometer data
        printf("Magnetic field in X-Axis: %d\n", xMag);
        printf("Magnetic field in Y-Axis: %d\n", yMag);
        printf("Magnetic field in Z-Axis: %d\n", zMag);

        sleep_ms(1000);
    }
}