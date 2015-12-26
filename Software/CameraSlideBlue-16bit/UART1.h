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
#define UART1_RX_BUFFER_SIZE        40

void UART1_init(void);

void UART1_writeLine(unsigned char* data, unsigned char len);

void UART1_writeChar(unsigned char chr);

unsigned char* UART1_readLine(unsigned char len);

unsigned char UART1_update(void);

#endif	/* UART1_H */

