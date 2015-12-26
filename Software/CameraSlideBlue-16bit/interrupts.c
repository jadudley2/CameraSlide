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

extern unsigned char UART1_rxBuffer[UART1_RX_BUFFER_SIZE];
extern unsigned char UART1_rxBufferWritePos;
extern unsigned char UART1_rxBufferCount;

extern unsigned char ADC_flag;

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

    // read buffered rx data into soft buffer
    UART1_rxBuffer[UART1_rxBufferWritePos] = U1RXREG;
    UART1_rxBufferCount++;      // increment buffer byte count
    UART1_rxBufferWritePos++;   // increment buffer write pos

    // check for write pos wraparound
    if (UART1_rxBufferWritePos >= UART1_RX_BUFFER_SIZE) {
        UART1_rxBufferWritePos = 0;
    }
}

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt(void) {
    ADC_flag = 1;
    IFS0bits.AD1IF = 0;
}