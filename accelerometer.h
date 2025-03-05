/*
 * accelerometer.h
 *
 *  Created on: 4 Mar 2025
 *      Author: mrcol
 */

#ifndef EEC172_FINALPROJECT_SRC_ACCELEROMETER_H_
#define EEC172_FINALPROJECT_SRC_ACCELEROMETER_H_

//****************************************************************************
//************************  IMU Acceleration Functions ***********************
//****************************************************************************

enum AccelAxis {
    X_Axis,
    Y_Axis,
    Z_Axis
};
typedef enum AccelAxis AccelAxis_t;

int8_t GetAccel(AccelAxis_t axis, int pixelLimit) {
    unsigned char address, offset, data;
    int8_t accel;

    switch (axis) {
        case X_Axis:
            address = 0x18;
            offset  = 0x3;
            break;
        case Y_Axis:
            address = 0x18;
            offset  = 0x5;
            break;
        case Z_Axis:
            address = 0x18;
            offset  = 0x7;
            break;
    }

    I2C_IF_Write(address,&offset,1,0);
    I2C_IF_Read(address, &data, 1);     // Read the specified length of data

    accel = ((int8_t)data * pixelLimit) / 128;  // 128 due to int8_t having a ceiling of 128

    switch (axis) {
        case Y_Axis:
            accel *= -1;
            break;
        default:
            break;
    }

    return accel;
}

#endif /* EEC172_FINALPROJECT_SRC_ACCELEROMETER_H_ */
