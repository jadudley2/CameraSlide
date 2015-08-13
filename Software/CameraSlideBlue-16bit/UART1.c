/*
 * File:   UART1.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:40 PM
 */

#include "UART1.h"

// Transmit buffer variables
unsigned char UART1_txBuffer[UART1_TX_BUFFER_SIZE] = { 0 };   // UART1 tx buffer
unsigned char UART1_txBufferCount;        // how many bytes in UART1_txBuffer
unsigned char UART1_txBufferWritePos;     // write position in UART1_txBuffer
unsigned char UART1_txBufferReadPos;      // read position in UART1_txBuffer

// Receive (user-defined) buffer variables
unsigned char UART1_rxBuffer[UART1_RX_BUFFER_SIZE];       // UART1 rx buffer
unsigned char UART1_rxBufferCount;        // how many bytes in UART1_rxBuffer
unsigned char UART1_rxBufferWritePos;     // write position in UART1_rxBuffer
unsigned char UART1_rxBufferReadPos;      // read position in UART1_rxBuffer

unsigned char UART1_txFlag;
unsigned char UART1_rxFlag;

void UART_init() {
    UART1_txBufferCount = 0;
    UART1_txBufferWritePos = 0;
    UART1_txBufferReadPos = 0;

    UART1_rxBufferCount = 0;
    UART1_rxBufferWritePos = 0;
    UART1_rxBufferReadPos = 0;

    UART1_txFlag = 0;
    UART1_rxFlag = 0;

    // set uart registers
    TRISBbits.TRISB2 = 1;       // set rx pin as input
    U1STAbits.UTXISEL0 = 0;     // set UT1ISEL interrupt select...
    U1STAbits.UTXISEL1 = 1;     // interrupt when transmit buffer is empty
    U1STAbits.URXISEL = 0;  // interrupt when any data transferred to rx buffer
    U1STAbits.ADDEN = 1;    // enable address detect (last data bit is 1)
    U1MODEbits.PDSEL = 0b00;    // 8 data bits, no parity
    U1MODEbits.STSEL = 0;       // 1 stop bit
    U1MODEbits.BRGH = 0;        // set BRGH to 0
    U1BRG = 12;     // set U1BRG to 12 (based on Fcy = 2MHz) for baud 9600
    U1MODEbits.UARTEN = 1;  // enable UART
    U1MODEbits.USIDL = 0;   // continue on idle
    U1MODEbits.RTSMD = 1;   // simplex mode (set 0 for flow control)
    U1MODEbits.UEN = 0b00;  // enable tx/rx, dio control of flow control pins
}

void UART1_writeLine(unsigned char* data) {
    unsigned char chr = 1;
    unsigned char i = 0;

    // iterate through array until null byte
    while(chr != '\0') {
        chr = data[i];          // read byte at position
        if(chr == '\0') break;     // if null, break out
        UART1_writeChar(chr);      // write char to uart1 tx buffer
        i++;
    }
    UART1_writeChar(13);            // carriage return
    UART1_writeChar(10);            // line feed
}

void UART1_writeChar(unsigned char chr) {
    // if not enabled, enable transmit
    if(U1STAbits.UTXEN != 1) {
        U1STAbits.UTXEN = 1;
    }

    // if tx buffer not full, add directly to it
    if(U1STAbits.UTXBF != 1) {
        U1TXREG += chr;
    } else {    // add to the soft tx buffer
        // if at end of buffer, wrap to beginning
        if(UART1_txBufferWritePos >= UART1_TX_BUFFER_SIZE) {
            UART1_txBufferWritePos = 0;
        }
        UART1_txBuffer[UART1_txBufferWritePos] = chr;  // add 'chr' to buffer
        UART1_txBufferCount++;          // increment buffer byte count
        UART1_txBufferWritePos++;       // increment write position
    }
}

unsigned char* UART1_readLine(unsigned char len) {
    // transfer number of desired bytes to data array
    unsigned char read[len] = { 0 };
    unsigned char i;
    for (i = 0; i < len; i++) {
        read[i] = UART1_rxBuffer[UART1_rxBufferReadPos];
        UART1_rxBufferReadPos++;
        // check for buffer read wraparound
        if (UART1_rxBufferReadPos >= UART1_RX_BUFFER_SIZE) {
            UART1_rxBufferReadPos = 0;
        }
    }

    UART1_rxBufferCount -= len;

    return read;
}

unsigned char UART1_update(void) {
    // check if transmit interrupt has occurred
    if (UART1_txFlag == 1) {
        UART1_txFlag = 0;
        // send next bit, if needed
        if (UART1_txBufferCount > 0) {
            // if read pos of buffer is beyond end of array, move to 0
            if (UART1_txBufferReadPos >= UART1_TX_BUFFER_SIZE) {
                UART1_txBufferReadPos = 0;
            }
            // write next char to tx reg
            U1TXREG += UART1_txBuffer[UART1_txBufferReadPos];
            UART1_txBufferCount--;      // decrement buffer byte count
            UART1_txBufferReadPos++;    // increment buffer read pos
        }
    }

    // check if receive interrupt has occurred
    if (UART1_rxFlag == 1) {
        UART1_rxFlag = 0;

        // check for buffer overflow
        if (U1STAbits.OERR != 0) {
            U1STAbits.OERR = 0;
        }
        
        // clear all bytes in buffer reg
        while(U1RXREG != 0) {
            unsigned char chr = U1RXREG;
            // add data to rxbuffer at rxwritepos
            if(UART1_rxBufferWritePos >= UART1_TX_BUFFER_SIZE) {
                UART1_rxBufferWritePos = 0;
            }
            UART1_rxBuffer[UART1_rxBufferWritePos] = chr;
            UART1_rxBufferWritePos++;           // increment buffer write pos
            UART1_rxBufferCount++;              // increment buffer byte count
        }
    }

    // return the number of bytes left in rx soft buffer
    return UART1_rxBufferCount;
}