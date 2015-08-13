/*
 * File:   motor.c
 * Author: jadudley
 *
 * Created on April 3, 2015, 8:05 PM
 */

#include "motor.h"

extern timeObj tReset;
timeObj tPrev;

unsigned char moveSingleStep(unsigned char dir) {
    timeObj tCurr = TMR32_getTime();

    // check if sufficient time has passed
    if(TMR32_timeDiff(tCurr, tPrev, MSEC) >= STEP_INTERVAL) {
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

void motor_init(void) {
    // set motor initial state
    MOTOR_1_PIN = HI;
    MOTOR_2_PIN = LO;
    MOTOR_3_PIN = LO;
    MOTOR_4_PIN = LO;
    
    // enable motor coils
    MOTOR_EN_PIN = HI;

    tPrev = tReset;
}