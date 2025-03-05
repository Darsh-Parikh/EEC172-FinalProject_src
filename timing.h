#ifndef TIMING_H
#define TIMING_H

#include "timer.h"

//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************

// the cc3200's fixed clock frequency of 80 MHz
// note the use of ULL to indicate an unsigned long long constant
#define SYSCLKFREQ 80000000ULL

// macro to convert ticks to microseconds
#define TICKS_TO_US(ticks) \
    ((((ticks) / SYSCLKFREQ) * 1000000ULL) + \
    ((((ticks) % SYSCLKFREQ) * 1000000ULL) / SYSCLKFREQ))\

// macro to convert microseconds to ticks
#define US_TO_TICKS(us) ((SYSCLKFREQ / 1000000ULL) * (us))

//--------------------

//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                3    /* Current Date */
#define MONTH               3     /* Month 1-12 */
#define YEAR                2025  /* Current year */
#define HOUR                14    /* Time - hours */
#define MINUTE              37    /* Time - minutes */
#define SECOND              0     /* Time - seconds */


#define TICK_RATE_MS   100      // how fast does the internal hardware timer tick
#define TICK_WAIT_RATE 1        // how many ticks does the system wait before executing it's loop again
//-------------------

static int set_time() {
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                          SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                          sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}

//------------------------------------------------------------------------

volatile int tick_count = 0;
int tick_time = 80000000/100;

void tickIncrement() {
    MAP_UtilsDelay(tick_time);
    tick_count++;
}

//------------------------------------------------------------------------

uint32_t timerTicks = 0;

// Timer Interrupt Handler
void TimerInterruptHandler(void) {
    // Clear the timer interrupt
    MAP_TimerIntClear(TIMERA0_BASE, TIMER_TIMA_TIMEOUT);

    timerTicks++;
}

void InitTimer(uint32_t interval_ms) {
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    TimerConfigure(TIMERA0_BASE, TIMER_CFG_PERIODIC);

    uint32_t ui32Period = (80000000 / 1000) * interval_ms; // Assuming 80MHz clock
    TimerLoadSet(TIMERA0_BASE, TIMER_A, ui32Period);

    TimerIntRegister(TIMERA0_BASE, TIMER_A, TimerInterruptHandler);
    TimerIntEnable(TIMERA0_BASE, TIMER_TIMA_TIMEOUT);

    TimerEnable(TIMERA0_BASE, TIMER_A);
}

void WaitTicks(int num_ticks) {
    uint32_t curr_ticks = timerTicks;
    while (timerTicks < (curr_ticks + num_ticks));
}

void ClearTicks() {
    timerTicks = 0;
}


#endif
