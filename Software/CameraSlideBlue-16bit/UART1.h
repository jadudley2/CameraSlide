/* 
 * File:   UART1.h
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:30 PM
 */

#ifndef UART1_H
#define	UART1_H

#include "constants.h"
#include "TMR32.h"

#define UART1_TX_BUFFER_SIZE        40
#define UART1_CMD_BUFFER_SIZE       10

typedef struct commands {
    unsigned char command;
    int value;
} command;

void UART1_init(void);

void UART1_writeLine(unsigned char* data, unsigned char len);

void UART1_writeChar(unsigned char chr);

command UART1_readCommand(void);

void UART1_sendCommand(unsigned char cmd, int val);

void UART1_flushCommands(void);

#endif	/* UART1_H */

