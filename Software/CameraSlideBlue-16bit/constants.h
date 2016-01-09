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
//#define ON                  1
//#define OFF                 0
#define HI                  1
#define LO                  0
#define HOME                0           // home switch
#define AWAY                1           // away switch
#define PI                  3.1425      // pi
#define OUTPUT              0
#define INPUT               1

/* digital pins */
#define HOME_SWITCH_PIN     PORTAbits.RA2
#define AWAY_SWITCH_PIN     PORTAbits.RA3

#define VCC_PWR_PIN         LATAbits.LATA0
#define SYS_LED_PIN         LATAbits.LATA1
#define HB_LED_PIN          LATBbits.LATB4
#define PWR_LED_PIN         LATAbits.LATA4

#define SHUTTER_PIN         LATBbits.LATB2

#define U1RX_PIN            LATBbits.LATB6
#define U1TX_PIN            LATBbits.LATB7
#define U1CTS_PIN           LATBbits.LATB8
#define U1RTS_PIN           LATBbits.LATB9
#define BT_RESET_PIN        LATBbits.LATB10

#define MOTOR_4_PIN         LATBbits.LATB11
#define MOTOR_3_PIN         LATBbits.LATB12
#define MOTOR_2_PIN         LATBbits.LATB13
#define MOTOR_1_PIN         LATBbits.LATB14
#define MOTOR_EN_PIN        LATBbits.LATB15

/* camera */
#define DEBOUNCE_TIME       50              // ms

/* commands */
#define CMD_BUFFER_SIZE     10

/* motor defaults */
#define MOTOR_SPEED         80
#define MOTOR_STEPS_PER_REV 200

/* hardware defaults */
#define PULLEY_RADIUS       15              // mm
#define SLIDE_LENGTH        914.4           // mm (914.4 mm = 36 in)

/* other defaults */
#define SHUTTER_TIME        1000               // ms
#define PROCESS_TIME        1000               // ms
#define TOTAL_PICS          40              
#define HB_INTERVAL         500            // ms
#define VOLTAGE_THRESHOLD   250             // out of 1024, scaled to 9.9V

#define MSG_START_BYTE      0x21
#define MSG_END_BYTE        0x22

#define MSG_BLANK           0x30
#define MSG_PICS            0x31
#define MSG_SHUTTER         0x32
#define MSG_PROCESS         0x33
#define MSG_SPEED           0x34
#define MSG_DIRECTION       0x35
#define MSG_SLIDE_LENGTH    0x36
#define MSG_RADIUS          0x37
#define MSG_ANG_STEP        0x38
#define MSG_RUN             0x39
#define MSG_ABORT           0x3A
#define MSG_MOVE_HOME       0x3B
#define MSG_MOVE_START      0x3C
#define MSG_HEARTBEAT       0x3D

#define MSG_TAKING_PIC      0x41
#define MSG_MOVING          0x42
#define MSG_PROCESSING      0x43

#define ACK_SUCCESS         0x51
#define ACK_FAILURE         0x52
#define ACK_INVALID         0x53
#define ACK_UNKNOWN         0x54

#endif	/* CONSTANTS_H */

