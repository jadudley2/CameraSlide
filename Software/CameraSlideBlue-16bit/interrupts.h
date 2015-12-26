/* 
 * File:   interrupts.h
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:47 PM
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "UART1.h"

void interrupts_init(void);

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void);

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void);

void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void);

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt(void);

#endif	/* INTERRUPTS_H */

