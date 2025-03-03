#ifndef TV_H
#define TV_H

#include "timing.h"

//*****************************************************************************
//                 SYSTICK
//*****************************************************************************

// systick reload value set to 40ms period
// (PERIOD_SEC) * (SYSCLKFREQ) = PERIOD_TICKS
#define SYSTICK_RELOAD_VAL 3200000UL

// track systick counter periods elapsed
// if it is not 0, we know the transmission ended
volatile int systick_cnt = 0;
volatile int sys_temp = 0;

/*
 * Reset SysTick Counter
 */
static inline void SysTickReset(void) {
    // any write to the ST_CURRENT register clears it
    // after clearing it automatically gets reset without
    // triggering exception logic
    // see reference manual section 3.2.1
    HWREG(NVIC_ST_CURRENT) = 1;

    // clear the global count variable
    systick_cnt = 0;
}

/**
 * SysTick Interrupt Handler
 *
 * Keep track of whether the systick counter wrapped
 */
static void SysTickHandler(void) {
    // increment every time the systick handler fires
    systick_cnt++;
}

static void SysTickInit(void) {
    // configure the reset value for the systick countdown register
    MAP_SysTickPeriodSet(SYSTICK_RELOAD_VAL);

    // register interrupts on the systick module
    MAP_SysTickIntRegister(SysTickHandler);

    // enable interrupts on systick
    // (trigger SysTickHandler when countdown reaches 0)
    MAP_SysTickIntEnable();

    // enable the systick module itself
    MAP_SysTickEnable();
}

//*****************************************************************************
//                 IR Reader
//*****************************************************************************

volatile unsigned long long deltas[50];
unsigned long long low_thresh;
unsigned long long high_thresh;
unsigned long long low_low;
unsigned long long low_high;
unsigned long long high_low;
unsigned long long high_high;

volatile unsigned long P64_intcount = 0;
volatile unsigned long P64_deltacount = 0;
volatile unsigned char P64_intflag = 0;

static void GPIOA1IntHandler(void) { //  handler
    volatile unsigned long ulStatus = MAP_GPIOIntStatus (GPIOA1_BASE, true);
    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);        // clear interrupts on GPIOA1
    if(systick_cnt) {
        P64_intcount = 0;
        sys_temp = systick_cnt;
        systick_cnt = 0;
        deltas[0] = 0;
        P64_deltacount = 0;
        SysTickReset();
    }
    deltas[P64_deltacount] = SYSTICK_RELOAD_VAL - SysTickValueGet();
    SysTickReset();
    if(deltas[0] > low_thresh && deltas[0] < high_thresh) {
        P64_deltacount++;
    }
    P64_intflag=1;
    P64_intcount++;
    return;
}

void InitIR() {
    MAP_GPIOIntRegister(GPIOA1_BASE, GPIOA1IntHandler);
    MAP_GPIOIntTypeSet(GPIOA1_BASE, 0x2, GPIO_RISING_EDGE);    // Pin 64

    volatile unsigned long ulStatus = MAP_GPIOIntStatus(GPIOA1_BASE, false);
    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);

    // Enable P60 interrupts
    MAP_GPIOIntEnable(GPIOA1_BASE, 0x2);


    //------- constants

    low_thresh  = US_TO_TICKS(4700);
    high_thresh = US_TO_TICKS(5500);
    low_low     = US_TO_TICKS(900);
    low_high    = US_TO_TICKS(1300);
    high_low    = US_TO_TICKS(2000);
    high_high   = US_TO_TICKS(2400);
}

char deltasToBtn(volatile unsigned long long* array) {
    int bits[32];
    int i;
    for (i = 1; i < 34; i++) {
        if (array[i] > low_low && array[i] < low_high) bits[i-1] = 0;
        else if (array[i] > high_low && array[i] < high_high) bits[i-1] = 1;
    }
    if (bits[0] != 0 || bits[1] != 0 || bits[2] != 0 || bits[3] != 1 || bits[4] != 0 || bits[5] != 0 || bits[6] != 0 || bits[7] != 0) return 0;
    if (bits[16] == 0 && bits[17] == 0 && bits[18] == 0 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '1';
    if (bits[16] == 1 && bits[17] == 0 && bits[18] == 0 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '2';
    if (bits[16] == 0 && bits[17] == 1 && bits[18] == 0 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '3';
    if (bits[16] == 1 && bits[17] == 1 && bits[18] == 0 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '4';
    if (bits[16] == 0 && bits[17] == 0 && bits[18] == 1 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '5';
    if (bits[16] == 1 && bits[17] == 0 && bits[18] == 1 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '6';
    if (bits[16] == 0 && bits[17] == 1 && bits[18] == 1 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '7';
    if (bits[16] == 1 && bits[17] == 1 && bits[18] == 1 && bits[19] == 0 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '8';
    if (bits[16] == 0 && bits[17] == 0 && bits[18] == 0 && bits[19] == 1 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '9';
    if (bits[16] == 1 && bits[17] == 0 && bits[18] == 0 && bits[19] == 1 && bits[20] == 0 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return '0';
    if (bits[16] == 1 && bits[17] == 0 && bits[18] == 1 && bits[19] == 1 && bits[20] == 1 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return 'L';
    if (bits[16] == 0 && bits[17] == 0 && bits[18] == 1 && bits[19] == 0 && bits[20] == 1 && bits[21] == 0 && bits[22] == 0 && bits[23] == 0) return 'M';
    return 0;
}

// returns the how long it has been between this reading and last reading
int ReadIR(char *out_char) {
    if (P64_deltacount >= 33) {
        int out_val = sys_temp;

        volatile unsigned long long* tempd = deltas;
        *out_char = deltasToBtn(tempd);

        P64_deltacount = 0;
        sys_temp = 0;

        return out_val;
    } else {
        *out_char = '\0';
        return 0;
    }
}

//*****************************************************************************
//                 TV Decoder
//*****************************************************************************

// Cycles through the different characters that each TV button represents.
// To prevent cycles and only output the first possible character option, set `reset` to 1
// Eg. `btn = 2` cycles between 'A','B','C' and back to 'A'. But if `reset = 1`, `btn = 2` will always return 'A'
char btnToChar(char btn, int reset) {
    static int count = 0;
    static int prev = 0;

    if (btn == '\0') {
        return '\0';
    }

    if (!reset && (btn == prev)) {
        count = (count + 1) % 4;
    } else {
        count = 0;
    }

    if (btn != '7' && btn != '9') {
        count = count % 3;
    }
    prev = btn;

    if (btn == '1') return '1';
    if (btn == '2' && count == 0) return 'A';
    if (btn == '2' && count == 1) return 'B';
    if (btn == '2' && count == 2) return 'C';
    if (btn == '3' && count == 0) return 'D';
    if (btn == '3' && count == 1) return 'E';
    if (btn == '3' && count == 2) return 'F';
    if (btn == '4' && count == 0) return 'G';
    if (btn == '4' && count == 1) return 'H';
    if (btn == '4' && count == 2) return 'I';
    if (btn == '5' && count == 0) return 'J';
    if (btn == '5' && count == 1) return 'K';
    if (btn == '5' && count == 2) return 'L';
    if (btn == '6' && count == 0) return 'M';
    if (btn == '6' && count == 1) return 'N';
    if (btn == '6' && count == 2) return 'O';
    if (btn == '7' && count == 0) return 'P';
    if (btn == '7' && count == 1) return 'Q';
    if (btn == '7' && count == 2) return 'R';
    if (btn == '7' && count == 3) return 'S';
    if (btn == '8' && count == 0) return 'T';
    if (btn == '8' && count == 1) return 'U';
    if (btn == '8' && count == 2) return 'V';
    if (btn == '9' && count == 0) return 'W';
    if (btn == '9' && count == 1) return 'X';
    if (btn == '9' && count == 2) return 'Y';
    if (btn == '9' && count == 3) return 'Z';
    if (btn == '0') return ' ';
    if (btn == 'M') return 'm';
    if (btn == 'L') return 'l';

    return '\0';
}

// modifies out_char to the value from the TV reading. sets it to '\0' in case of no press
// return -1 if no press, 0 if the press was to cycle between characters, and 1 if it's an entirely new press
int GetTVPress(char *out_char) {
    static char btn  = '\0';
    static char prev = '\0';

    int time_stamp = ReadIR(&btn);

    int new_press = (time_stamp > 10) || (btn != prev);
    prev = btn;

    *out_char = btnToChar(btn, new_press);

    if (!(*out_char)) {
        return -1;
    } else {
        return new_press;
    }
}


#endif
