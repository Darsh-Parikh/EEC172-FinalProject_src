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
#include "buffers.h"
#include "web-server.h"
#include "oled.h"
#include "console_display.h"
#include "accelerometer.h"

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

#define CONTROLLER  0
#define CONSOLE     1

#define TARGET      CONTROLLER

void controller_main();
void console_main();
void old_main();

void main() {
    BoardInit();
    PinMuxConfig();

    InitTerm();
    ClearTerm();
    UART_PRINT("My terminal works!\n\r");

#if TARGET == CONTROLLER
    controller_main();
#elif TARGET == CONSOLE
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
}

void console_main() {
    InitComm(1);
    InitAWS();

}

void old_main() {
    while (1) {
        Report("????????\r\n");
    }
}

