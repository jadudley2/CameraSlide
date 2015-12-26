/*
 * File:   motor.c
 * Author: jadudley
 *
 * Created on April 3, 2015, 8:05 PM
 */

#include "motor.h"

unsigned int motor_speed;           // interval between steps in ms
unsigned int motor_stepsPerRev;     // steps per motor revolution

extern timeObj tReset;
timeObj tPrev;

unsigned char moveSingleStep(unsigned char dir) {
    timeObj tCurr = TMR32_getTime();

    // check if sufficient time has passed
    if(TMR32_timeDiff(tCurr, tPrev, MSEC) >= motor_speed) {
        if(MOTOR_1_PIN == HI) {
            if(dir == HOME) {
                MOTOR_2_PIN = HI;
            } else {
                MOTOR_4_PIN = HI;
            }
            MOTOR_1_PIN = LO;
        } else if(MOTOR_2_PIN == HI) {
            if(dir == HOME) {
                MOTOR_3_PIN = HI;
            } else {
                MOTOR_1_PIN = HI;
            }
            MOTOR_2_PIN = LO;
        } else if(MOTOR_3_PIN == HI) {
            if(dir == HOME) {
                MOTOR_4_PIN = HI;
            } else {
                MOTOR_2_PIN = HI;
            }
            MOTOR_3_PIN = LO;
        } else if(MOTOR_4_PIN == HI) {
            if(dir == HOME) {
                MOTOR_1_PIN = HI;
            } else {
                MOTOR_3_PIN = HI;
            }
            MOTOR_4_PIN = LO;
        } else {
            // was unpowered
            MOTOR_1_PIN = HI;
        }

        // successful step
        tPrev = tCurr;
        return 1;
    }
    return 0;
}

void motor_init(unsigned int spd, unsigned int stepsPerRev) {
    // set motor initial state
    MOTOR_1_PIN = HI;
    MOTOR_2_PIN = LO;
    MOTOR_3_PIN = LO;
    MOTOR_4_PIN = LO;
    
    // enable motor coils
    MOTOR_EN_PIN = HI;

    tPrev = tReset;
    motor_stepsPerRev = stepsPerRev;
    motor_speed = spd;
}