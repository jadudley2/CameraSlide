/* 
 * File:   TMR32.h
 * Author: James Dudley
 *
 * Created on May 19, 2015, 12:03 PM
 */

#ifndef TMR32_H
#define	TMR32_H

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define SEC                 1
#define MSEC                1000
#define USEC                1000000
#define TICKS_PER_S         2000000
#define US_PER_MS           1000
#define S_PER_OVERFLOW      2000
#define MS_PER_S            1000

#define TMR32_PRSCL         0b00        // 1:1 prescale
#define TMR32_PRIORITY      0b101       // priority level 5

typedef struct times {
    char ovrflws;
    unsigned long ticks;
} timeObj;

unsigned long TMR32_timeDiff(timeObj t1, timeObj t2, long scale);

timeObj TMR32_getTime(void);

void TMR32_init(void);

#endif	/* TMR32_H */

