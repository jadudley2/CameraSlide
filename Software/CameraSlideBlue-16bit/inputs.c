/*
 * File:   inputs.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:10 PM
 */

#include "inputs.h"

timeObj inputStartTimes[NUM_INPUTS];
extern timeObj tReset;
unsigned char j;

unsigned char checkInputState(unsigned char input) {
    unsigned char aggregate = 0;
    for(j = 0; j < NUM_AGGREGATE_SAMPLES; j++) {
        // add pin state to aggregate
        switch(input) {
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