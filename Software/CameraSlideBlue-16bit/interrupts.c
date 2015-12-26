/*
 * File:   interrupts.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:02 PM
 */

#include "interrupts.h"

extern unsigned char TMR32_overflows;

extern unsigned char UART1_txFlag;
extern unsigned char UART1_rxFlag;

extern unsigned char ADC_flag;

void interrupts_init() {
    INTCON1bits.NSTDIS = 1;             // disable nested interrupts
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    TMR32_overflows++;
    IFS0bits.T2IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void) {
    UART1_rxFlag = 1;
    IFS0bits.U1RXIF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _AD1Interrup(void) {
    ADC_flag = 1;
    IFS0bits.AD1IF = 0;
}