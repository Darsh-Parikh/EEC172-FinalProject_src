#ifndef TIMING_H
#define TIMING_H

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




#endif
