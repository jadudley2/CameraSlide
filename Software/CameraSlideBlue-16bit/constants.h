/* 
 * File:   constants.h
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:12 PM
 */

#ifndef CONSTANTS_H
#define	CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/* general */
#define ON                  1
#define OFF                 0
#define HI                  1
#define LO                  0
#define HOME                0           // home switch
#define AWAY                1           // away switch

/* digital pins */
#define HOME_SWITCH_PIN     PORTAbits.RA4
#define AWAY_SWITCH_PIN     PORTBbits.RB4

#define MOTOR_1_PIN         LATBbits.LATB6
#define MOTOR_2_PIN         LATAbits.LATA7
#define MOTOR_3_PIN         LATBbits.LATB9
#define MOTOR_4_PIN         LATBbits.LATB8
#define MOTOR_EN_PIN        LATBbits.LATB7

#define SHUTTER_PIN         LATBbits.LATB5

/* camera */
#define DEBOUNCE_TIME       50              // ms

#endif	/* CONSTANTS_H */

