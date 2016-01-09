/*
 * File:   interrupts.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:02 PM
 */

#include "interrupts.h"

extern unsigned char TMR32_overflows;

extern unsigned char UART1_txBufferCount;
extern unsigned char UART1_txBufferReadPos;
extern unsigned char UART1_txBuffer[UART1_TX_BUFFER_SIZE];

extern unsigned char UART1_cmdFlag;
extern unsigned char UART1_cmdBufferCount;
extern unsigned char UART1_cmdBufferWritePos;
extern command UART1_cmdBuffer[UART1_CMD_BUFFER_SIZE];
extern command resetCmd;

extern unsigned char ADC_flag;

unsigned char startFlag = 0;
unsigned char msgFlag = 0;
unsigned char valFlag = 0;
command cmd = {0,0};

void interrupts_init() {
    INTCON1bits.NSTDIS = 1;             // disable nested interrupts
}

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void) {
    TMR32_overflows++;
    IFS0bits.T2IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;        // reset interrupt flag
    
    // send next bit, if there's more in the buffer
    if (UART1_txBufferCount > 0) {
        // make sure transmit buffer isn't full
        if (U1STAbits.UTXBF != 1) {
            // check for buffer read wraparound
            if (UART1_txBufferReadPos >= UART1_TX_BUFFER_SIZE) {
                UART1_txBufferReadPos = 0;
            }

            // set tx enable bit
            U1STAbits.UTXEN = 1;

            // write next char to tx reg
            U1TXREG = UART1_txBuffer[UART1_txBufferReadPos];
            UART1_txBufferCount--;      // decrement buffer byte count
            UART1_txBufferReadPos++;    // increment buffer read pos
        }
    }
}

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;        // reset interrupt flag

    // check for buffer overflow
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;     // reset buffer flag
    }

    // read buffered rx data
    unsigned char chr = U1RXREG;

    if (startFlag == 0) {
        msgFlag = 0;
        valFlag = 0;
        if (chr == MSG_START_BYTE) {
            startFlag = 1;
            cmd = resetCmd;
        }
    } else if (msgFlag == 0 && valFlag == 0) {
        cmd.command = chr;
        msgFlag = 1;
    } else if (msgFlag == 1 && valFlag == 0) {
        cmd.value = chr;
        valFlag = 1;
    } else if (msgFlag == 1 && valFlag == 1) {
        if (chr == MSG_END_BYTE) {
            UART1_cmdBuffer[UART1_cmdBufferWritePos] = cmd;
            UART1_cmdBufferWritePos++;
            if (UART1_cmdBufferWritePos >= UART1_CMD_BUFFER_SIZE) {
                UART1_cmdBufferWritePos = 0;
            }
            UART1_cmdBufferCount++;
            UART1_cmdFlag = 1;      // notify new command in buffer
        }
        startFlag = 0;
    }
}

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt(void) {
    ADC_flag = 1;
    IFS0bits.AD1IF = 0;
}