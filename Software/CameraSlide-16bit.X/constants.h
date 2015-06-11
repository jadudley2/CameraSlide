/* 
 * File:   constants.h
 * Author: James Dudley
 *
 * Created on May 23, 2015, 11:13 AM
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
#define HOME                6           // home switch
#define AWAY                7           // away switch

/* digital pins */
#define START_BUTTON_PIN    PORTAbits.RA2
#define CENTER_BUTTON_PIN   PORTAbits.RA3
#define UP_BUTTON_PIN       PORTBbits.RB2
#define DOWN_BUTTON_PIN     PORTBbits.RB3

#define HOME_SWITCH_PIN     PORTAbits.RA4
#define AWAY_SWITCH_PIN     PORTBbits.RB4

#define LCD_EN_PIN          LATBbits.LATB10
#define LCD_RS_PIN          LATBbits.LATB11
#define LCD_D4_PIN          LATBbits.LATB12
#define LCD_D5_PIN          LATBbits.LATB13
#define LCD_D6_PIN          LATBbits.LATB14
#define LCD_D7_PIN          LATBbits.LATB15

#define MOTOR_1_PIN         LATBbits.LATB6
#define MOTOR_2_PIN         LATAbits.LATA7
#define MOTOR_3_PIN         LATBbits.LATB9
#define MOTOR_4_PIN         LATBbits.LATB8
#define MOTOR_EN_PIN        LATBbits.LATB7

#define SHUTTER_PIN         LATBbits.LATB5

/* hardware */
#define HOME_POS            0
#define AWAY_POS            6250

/* camera */
#define NUM_SHUTTER_SPEEDS      55

/* LCD */
#define LCD_WIDTH               16
#define SHUTTER_STR_LENGTH      6
#define DIR_STR_LENGTH          4

#endif	/* CONSTANTS_H */

