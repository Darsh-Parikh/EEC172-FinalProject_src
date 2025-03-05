/*
 * controller.h
 *
 *  Created on: 5 Mar 2025
 *      Author: mrcol
 */
#include "buffers.h"

#ifndef EEC172_FINALPROJECT_SRC_CONTROLLER_H_
#define EEC172_FINALPROJECT_SRC_CONTROLLER_H_

int control() {
    GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_IN);
    char accelX;
    char accelY;
    int button1state;
    int button2state;
    char tv_press;
    int ret_val;
    int new_press;
    while (1) {
        WaitTicks(TICK_WAIT_RATE);
        ClearTicks();

        Report("1 run, took %d * 100 ms\r\n", timerTicks);
        // read buttons
        button1state = (GPIOPinRead(GPIOA1_BASE, 0x20) & 0x20);
        button2state = (GPIOPinRead(GPIOA2_BASE, 0x40) & 0x40);
        // read Accelerometer
        char accelX = GetAccel(X_Axis, X_Accel_PixelLimit),
        char accelY = GetAccel(Y_Axis, Y_Accel_PixelLimit);
        // read IR
        ret_val = GetTVPress(&tv_press);
        tvBtnSt = (ret_val != -1);
        // Send over UART
        StoreControllerData(button1State, button2State, accelX, accelY, tvBtn, tvBtnSt);
    }
}
#endif /* EEC172_FINALPROJECT_SRC_CONTROLLER_H_ */
