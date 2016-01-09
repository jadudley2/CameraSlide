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
unsigned char UART1_cmdFlag;
command UART1_cmdBuffer[UART1_CMD_BUFFER_SIZE];
unsigned char UART1_cmdBufferCount = 0;
unsigned char UART1_cmdBufferWritePos = 0;
unsigned char UART1_cmdBufferReadPos = 0;

command resetCmd = {MSG_BLANK, 0x00};

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
    IPC2bits.U1RXIP = 2;        // rx interrupt priority
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
    if (UART1_txBufferWritePos >= UART1_TX_BUFFER_SIZE) {
        UART1_txBufferWritePos = 0;
    }

    // if buffer not full, write character directly to tx register
    if (U1STAbits.UTXBF != 1) {
        U1STAbits.UTXEN = 1;
        U1TXREG = chr;
    } else {        // else, add to buffer
        UART1_txBuffer[UART1_txBufferWritePos] = chr;  // add char to buffer
        UART1_txBufferCount++;          // increment buffer byte count
        UART1_txBufferWritePos++;       // increment write position
    }
}

command UART1_readCommand(void) {
    command cmd = resetCmd;
    
    if (UART1_cmdBufferCount > 0) {
        cmd = UART1_cmdBuffer[UART1_cmdBufferReadPos];
        UART1_cmdBufferCount--;
        UART1_cmdBufferReadPos++;
        if (UART1_cmdBufferReadPos >= UART1_CMD_BUFFER_SIZE) {
            UART1_cmdBufferReadPos = 0;
        }
    }

    if (UART1_cmdBufferCount == 0) {
        UART1_cmdFlag = 0;
    }
    
    return cmd;
}

void UART1_sendCommand(unsigned char cmd, int val) {
    unsigned char line[] = {MSG_START_BYTE,cmd,val>>8,val,MSG_END_BYTE};
    UART1_writeLine(&line[0],5);
}

void UART1_flushCommands(void) {
    UART1_cmdFlag = 0;
    UART1_cmdBufferCount = 0;
    UART1_cmdBufferReadPos = 0;
    UART1_cmdBufferWritePos = 0;
}