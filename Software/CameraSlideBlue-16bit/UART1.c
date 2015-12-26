/*
 * File:   UART1.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:40 PM
 */

#include "UART1.h"

// Transmit buffer variables
unsigned char UART1_txBuffer[UART1_TX_BUFFER_SIZE];   // UART1 tx buffer
unsigned char UART1_txBufferCount = 0;      // how many bytes in UART1_txBuffer
unsigned char UART1_txBufferWritePos = 0;   // write position in UART1_txBuffer
unsigned char UART1_txBufferReadPos = 0;    // read position in UART1_txBuffer

// Receive (user-defined) buffer variables
unsigned char UART1_rxBuffer[UART1_RX_BUFFER_SIZE]; // UART1 rx buffer
unsigned char UART1_rxBufferCount = 0;      // how many bytes in UART1_rxBuffer
unsigned char UART1_rxBufferWritePos = 0;   // write position in UART1_rxBuffer
unsigned char UART1_rxBufferReadPos = 0;    // read position in UART1_rxBuffer

unsigned char UART1_rxFlag = 0;

unsigned char read[UART1_RX_BUFFER_SIZE];

void UART1_init() {
    // set uart registers
    PMD1bits.U1MD = 0;          // enable UART1 peripheral
    U1MODEbits.USIDL = 1;       // disable UART1 in idle mode
    U1MODEbits.RTSMD = 1;       // simplex mode (set 0 for flow control)
    U1MODEbits.PDSEL = 0b00;    // 8 data bits, no parity
    U1MODEbits.STSEL = 0;       // 1 stop bit
    U1MODEbits.BRGH = 0;        // set BRGH to 0
    U1BRG = 23;     // set U1BRG to 12 (based on Fcy = 2MHz) for baud 9600
    U1STAbits.UTXISEL0 = 0;     // set UT1ISEL interrupt select...
    U1STAbits.UTXISEL1 = 1;     // interrupt when transmit buffer is empty
    U1STAbits.URXISEL = 0b00;   // interrupt when any data in rx buffer
    IPC3bits.U1TXIP = 4;        // tx interrupt priority
    IPC2bits.U1RXIP = 4;        // rx interrupt priority
    IEC0bits.U1TXIE = 1;        // enable tx interrupt
    IEC0bits.U1RXIE = 1;        // enable rx interrupt
    U1STAbits.ADDEN = 1;        // enable address detect (last data bit is 1)
    U1MODEbits.UEN = 0b00;      // enable U1Tx, U1Rx in simplex mode
    U1MODEbits.UARTEN = 1;      // enable UART1
}

void UART1_writeLine(unsigned char* data, unsigned char len) {
    unsigned char i;

    // iterate through array until null byte
    for (i = 0; i < len; i++) {
        UART1_writeChar(data[i]);      // write char to uart1 tx buffer
    }
    UART1_writeChar('\r');            // carriage return
    UART1_writeChar('\n');            // line feed
}

void UART1_writeChar(unsigned char chr) {
    // if at end of buffer, wrap to beginning
    if(UART1_txBufferWritePos >= UART1_TX_BUFFER_SIZE) {
        UART1_txBufferWritePos = 0;
    }
    UART1_txBuffer[UART1_txBufferWritePos] = chr;  // add char to buffer
    UART1_txBufferCount++;          // increment buffer byte count
    UART1_txBufferWritePos++;       // increment write position
}

unsigned char* UART1_readLine(unsigned char len) {
    // bound len
    if ((len == 0) || (len > UART1_rxBufferCount)) {
        len = UART1_rxBufferCount;
    }
    
    // transfer number of desired bytes to data array
    unsigned char i;
    for (i = 0; i < len; i++) {
        read[i] = UART1_rxBuffer[UART1_rxBufferReadPos];
        UART1_rxBuffer[UART1_rxBufferReadPos] = 0;
        UART1_rxBufferCount--;
        UART1_rxBufferReadPos++;
        // check for buffer read pos wraparound
        if (UART1_rxBufferReadPos >= UART1_RX_BUFFER_SIZE) {
            UART1_rxBufferReadPos = 0;
        }
    }

    return &read[0];
}

unsigned char UART1_update(void) {
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

    // check if rx interrupt has occurred
    if (UART1_rxFlag == 1) {
        UART1_rxFlag = 0;

        // check for buffer overflow
        if (U1STAbits.OERR == 1) {
            U1STAbits.OERR = 0;     // reset buffer flag
        }

        // read through entire rx fifo buffer
        while (U1STAbits.URXDA != 0) {
            // read buffered rx data into soft buffer
            UART1_rxBuffer[UART1_rxBufferWritePos] = U1RXREG;
            UART1_rxBufferCount++;      // increment buffer byte count
            UART1_rxBufferWritePos++;   // increment buffer write pos

            // check for right pos wraparound
            if (UART1_rxBufferWritePos >= UART1_RX_BUFFER_SIZE) {
                UART1_rxBufferWritePos = 0;
            }
        }
    }

    // return the number of bytes in rx soft buffer
    return UART1_rxBufferCount;
}