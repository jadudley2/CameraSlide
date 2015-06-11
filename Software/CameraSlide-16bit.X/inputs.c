/*
 * File:   inputs.c
 * Author: James Dudley
 *
 * Created on May 23, 2015, 12:16 PM
 */

#include "inputs.h"

timeObj inputStartTimes[NUM_INPUTS];
extern timeObj tReset;
unsigned char buttonFlags[NUM_BUTTONS];
unsigned char j;

unsigned char checkButtons(void) {
    unsigned char btn;
    for(btn = 0; btn < NUM_BUTTONS; btn++) {
        // check if button is being held past debounce time
        if(checkSingleInput(btn) == HI) {
            buttonFlags[btn] = HI;
            // check for long press
            if(btn == START_BUTTON_LONG) {
                buttonFlags[START_BUTTON] = LO;
                inputStartTimes[START_BUTTON] = tReset;
            } else if(btn == CENTER_BUTTON_LONG) {
                buttonFlags[CENTER_BUTTON] = LO;
                inputStartTimes[CENTER_BUTTON] = tReset;
            }
        } else {
            // check if button has just been released
            if(buttonFlags[btn] == HI) {
                // if so, reset and return that button
                buttonFlags[btn] = LO;
                inputStartTimes[btn] = tReset;
                return btn;
            }
        }
    }
    return NO_INPUT;
}

unsigned char checkSingleInput(unsigned char input) {
    timeObj tCurr = TMR32_getTime();
    timeObj tStart = inputStartTimes[input];

    // check if pin is hi or lo
    if(checkInputState(input) == HI) {
        // check if tStart has been reset
        if(tStart.ovrflws == tReset.ovrflws) {
            // input is newly pressed, set start time to curr
            inputStartTimes[input] = tCurr;
        } else {
            if((input != START_BUTTON_LONG) && (input != CENTER_BUTTON_LONG)) {
                // if t since start is longer than debounce, input is pressed
                if(TMR32_timeDiff(tCurr, tStart, MSEC) > DEBOUNCE_TIME) {
                    return HI;
                }
            } else {
                // if t since start is longer than debounce, input is pressed
                if(TMR32_timeDiff(tCurr, tStart, MSEC) > LONG_PRESS_TIME) {
                    return HI;
                }
            }
        }
    } else {
        // reset input start time
        inputStartTimes[input] = tReset;
    }
    return LO;
}

unsigned char checkInputState(unsigned char input) {
    unsigned char aggregate = 0;
    for(j = 0; j < NUM_AGGREGATE_SAMPLES; j++) {
        // add pin state to aggregate
        switch(input) {
            case START_BUTTON:
                // OR
            case START_BUTTON_LONG:
                aggregate += START_BUTTON_PIN;
                break;
            case CENTER_BUTTON:
                // OR
            case CENTER_BUTTON_LONG:
                aggregate += CENTER_BUTTON_PIN;
                break;
            case UP_BUTTON:
                aggregate += UP_BUTTON_PIN;
                break;
            case DOWN_BUTTON:
                aggregate += DOWN_BUTTON_PIN;
                break;
            case HOME_SWITCH:
                aggregate += HOME_SWITCH_PIN;
                break;
            case AWAY_SWITCH:
                aggregate += AWAY_SWITCH_PIN;
                break;
            default:
                // unknown input
                break;
        }
    }

    if(aggregate < AGGREGATE_THRESHOLD) {
        aggregate = LO;
    } else {
        aggregate = HI;
    }

    return aggregate;
}

void inputs_init(void) {
    for(j = 0; j < NUM_INPUTS; j++) {
        inputStartTimes[j] = tReset;
    }
}