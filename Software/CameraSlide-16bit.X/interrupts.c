/*
 * File:   interrupts.c
 * Author: James Dudley
 *
 * Created on May 19, 2015, 12:28 PM
 */

#include "interrupts.h"

extern unsigned char TMR32_overflows;

void interrupts_init() {
    INTCON1bits.NSTDIS = 1;             // disable nested interrupts
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    TMR32_overflows++;
    IFS0bits.T2IF = 0;
}