/*
 * controller.h
 *
 *  Created on: 5 Mar 2025
 *      Author: mrcol
 */
#include "buffers.h"

#ifndef EEC172_FINALPROJECT_SRC_CONTROLLER_H_
#define EEC172_FINALPROJECT_SRC_CONTROLLER_H_

int Control() {
    GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_IN);
    int accelX;
    int accelY;
    int button1state;
    int button2state;
    char tv_press = '0';
    char tv_press_prev;
    int ret_val;
    int new_press;
    while (1) {
        WaitTicks(TICK_WAIT_RATE);
        ClearTicks();

        //Report("1 run, took %d * 100 ms\r\n", timerTicks);
        // read buttons
        button1state = !!(GPIOPinRead(GPIOA1_BASE, 0x20) & 0x20);
        button2state = !!(GPIOPinRead(GPIOA2_BASE, 0x40) & 0x40);
        // read Accelerometer
        accelX = GetAccel(X_Axis, X_Accel_PixelLimit),
        accelY = GetAccel(Y_Axis, Y_Accel_PixelLimit);
        // read IR
        tv_press_prev = tv_press;
        ret_val = GetTVPress(&tv_press);
        if(tv_press == '\0') tv_press = '0';
        new_press = (ret_val != -1);
        if(!new_press) tv_press = tv_press_prev;
        // Send over UART
        StoreControllerData(button1state, button2state, accelX, accelY, tv_press, new_press);
        Report("but1: %d, but2: %d, accx: %d, accy: %d, btn: %c, btnst: %d",button1state, button2state, accelX, accelY, tv_press, new_press);
        Report("\r\n");
    }
}
#endif /* EEC172_FINALPROJECT_SRC_CONTROLLER_H_ */
