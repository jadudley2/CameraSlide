/* 
 * File:   interrupts.h
 * Author: James Dudley
 *
 * Created on May 19, 2015, 12:28 PM
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

void interrupts_init(void);

void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void);

void __attribute__((__interrupt__, auto_psv)) _ADC1Interrupt(void);

void __attribute__((__interrupt__, auto_psv)) _INT0Interrupt(void);

#endif	/* INTERRUPTS_H */

