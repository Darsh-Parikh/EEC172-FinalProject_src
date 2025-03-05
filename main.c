// Standard includes
#include <stdio.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_common_reg.h"
#include "hw_ints.h"
#include "hw_apps_rcm.h"
#include "hw_nvic.h"
#include "hw_memmap.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "gpio.h"
#include "systick.h"
#include "i2c_if.h"
#include <stdint.h>

//Common interface includes
#include "pin_mux_config.h"
#include "gpio_if.h"
#include "common.h"
#include "uart_if.h"

// Custom includes
#include "../utils/network_utils.h"
#include "tv.h"
#include "timing.h"
#include "oled.h"
#include "console_display.h"
#include "accelerometer.h"
#include "game_states.h"

//*****************************************************************************
//                 GLOBAL  -- Start
//*****************************************************************************

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
volatile int g_iCounter = 0;

extern void (* const g_pfnVectors[])(void);

static void BoardInit(void) {
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//                 GLOBAL  -- End: df
//*****************************************************************************

//-----------------------------

#define TICK_RATE_MS   100      // how fast does the internal hardware timer tick
#define TICK_WAIT_RATE 10       // how many ticks does the system wait before executing it's loop again

#define CONTROLLER_MCU  0
#define CONSOLE_MCU     1

#define TARGET          CONTROLLER_MCU

void controller_main();
void console_main();
void old_main();

void main() {
    BoardInit();
    PinMuxConfig();

    InitTerm();
    ClearTerm();
    UART_PRINT("My terminal works!\n\r");

#if TARGET == CONTROLLER_MCU
    controller_main();
#elif TARGET == CONSOLE_MCU
    console_main();
#else
    old_main();
#endif
}

//------------------------------------------------

void controller_main() {
    InitComm(0);

    SysTickInit();
    InitIR();

    I2C_IF_Open(I2C_MASTER_MODE_FST);   // setup I2C for Accelerometer

    InitTimer(TICK_RATE_MS);
    while (1) {
        WaitTicks(TICK_WAIT_RATE);
        ClearTicks();

        Report("1 run, took %d * 100 ms\r\n", timerTicks);

        // read Buttons

        // read Accelerometer

        // read IR

        // Send over UART
    }


}

void console_main() {
    InitComm(1);
    InitOLED();

    // I2C_IF_Open(I2C_MASTER_MODE_FST); display();     // <-- Uncomment this to test dispaly working

    GameStates state = GAME_LOOP;
    ClearBuffer(&playerName);

    InitAWS();
    FetchScoresFromAWS();

    InitTimer(TICK_RATE_MS);
    while (1) {
        WaitTicks(TICK_WAIT_RATE);
        ClearTicks();

        int ret_val = -1;
        switch (state) {
            case START_SCREEN:
               ret_val = Run_StartScreen();
               if (ret_val == 1) {
                   // InitGame();
                   state = GAME_LOOP;
               }
               break;
            case GAME_LOOP:
               ret_val = Run_GameLoop();
               if (ret_val == 1) {
                   state = SCOREBOARD;
               }
               break;
            case SCOREBOARD:
               ret_val = Run_ScoreBoard();
               if (ret_val == 2) {
                   ClearBuffer(&playerName);
                   state = START_SCREEN;
               } else if (ret_val == 1) {
                   // InitGame();
                   state = GAME_LOOP;

               }
               break;
        }
    }
}

void old_main() {
    while (1) {
        Report("????????\r\n");
    }
}

