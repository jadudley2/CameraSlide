/*
 * File:   TMR32.c
 * Author: James Dudley
 *
 * Created on May 19, 2015, 12:03 PM
 */

#include "TMR32.h"

unsigned char TMR32_overflows = 0;
timeObj tReset = {-1, 0};

unsigned long TMR32_timeDiff(timeObj t1, timeObj t2, long scale) {
    timeObj tHi, tLo;
    float dTime = 0;

    // sort out which timeObj is greater
    if(t1.ovrflws > t2.ovrflws) {
        tHi = t1;
        tLo = t2;
    } else if(t2.ovrflws > t1.ovrflws) {
        tHi = t1;
        tLo = t2;
    } else {
        if(t1.ticks > t2.ticks) {
            tHi = t1;
            tLo = t2;
        } else {
            tHi = t2;
            tLo = t1;
        }
    }

    dTime = (tHi.ovrflws - tLo.ovrflws) * (S_PER_OVERFLOW * scale) +
        (tHi.ticks - tLo.ticks) / (TICKS_PER_S / scale);

    return dTime;
}

timeObj TMR32_getTime() {
    timeObj t;
    unsigned int lsw = 0;
    unsigned long msw = 0;

    t.ovrflws = TMR32_overflows;
    
    lsw = TMR2;
    msw = TMR3;
    t.ticks = lsw + (msw<<16);

    return t;
}

void TMR32_init() {
    T2CONbits.T32 = 1;                  // select 32 bit mode
    T2CONbits.TCKPS = TMR32_PRSCL;      // set timer prescale
    T2CONbits.TCS = 0;                  // clock source = Fosc/4
    T2CONbits.TGATE = 0;                // timer gate disabled
    PR2 = 0x2800;                       // set timer period, lsw (4e9)
    PR3 = 0xEE6B;                       // set timer period, msw (4e9)

    IEC0bits.T2IE = 1;                  // enable interrupt for timer
    IPC1bits.T2IP = TMR32_PRIORITY;     // set timer interrupt priority
    IFS0bits.T2IF = 0;                  // reset timer interrupt flag
    
    T2CONbits.TON = 1;                  // start timer
}