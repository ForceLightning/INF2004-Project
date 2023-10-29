/**
 * @file ir_sensor.h
 * @author Ang Jia Yu
 * @brief Header file for the IR sensor driver.
 * @version 0.1
 * @date 2023-10-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef IR_SENSOR_H
#define IR_SENSOR_H

// Function prototypes
//
void setupADCPins(uint adcPin);
void readBarcode();
#endif // IR_SENSOR_H

// End of driver/ir_sensor/ir_sensor.h.