/*
 * File:   statemachines.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:26 PM
 */

#include "statemachines.h"

extern timeObj tReset;
timeObj tShutterStart, tCurr;

unsigned int tProcess, lastPic, currPic, totPics, intSteps, startPos, currPos,
        nextPos, shutterTime, homePos, awayPos;

unsigned char travDir, mainState, runState, movingFlag, shtrFlag, picFlag, i;

void SM_mainStateMachine(void) {
    while(ON) {
        switch(mainState) {
            case STARTUP:
                mainState = IDLE;
                break;
            case IDLE:
                // TODO: check if bluetooth has sent command
                break;
            case FOCUS:
                // TODO: check if bluetooth has sent command
                break;
            case MOVING_TO_HOME:
                // OR
            case MOVING_TO_START:
                // TODO: update bluetooth on status
                // TODO: check if bluetooth has sent command
                
                // start moving
                movingFlag = HI;
                // check end limit switches
                unsigned char s = travDir;
                if(checkInputState(s) == HI) {
                    movingFlag = LO;            // no longer moving
                    if(mainState == MOVING_TO_HOME) {
                        currPos = homePos;     // at home
                        mainState = IDLE;       // go to idle state
                    } else {
                        currPos = startPos;     // at start
                        mainState = FOCUS;   // go to focusing state
                    }
                } else {
                    // keep moving
                    moveSingleStep(travDir);
                }
                break;
            case RUNNING:
                // clear all running flags and times
                shtrFlag = LO;
                movingFlag = LO;
                tShutterStart = tReset;
                currPic = 1;
                
                // intialize running state to taking pic
                runState = R_TAKING_PIC;

                // run the running state machine
                SM_runStateMachine();
                break;
            default:
                // invalid main state option
                break;
        }
    }
}

void SM_runStateMachine(void) {
    while(mainState == RUNNING) {
        // TODO: check if command from bluetooth
        // TODO: send update to bluetooth

        switch(runState) {
            case R_TAKING_PIC:
                if(shtrFlag == LO) {
                    if(picFlag == LO) {
                        SHUTTER_PIN = HI;               // initiate shutter
                        shtrFlag = HI;                  // shutter has been pressed
                        tShutterStart = TMR32_getTime();    // start shutter debounce
                    } else {
                        if(TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) > shutterTime) {
                            movingFlag = LO;            // get ready to move
                            currPic++;                  // pic has been snapped
                            picFlag = LO;
                            runState = R_TRAVELLING;    // move to next position
                        } else {
                            // wait for pic to be taken
                        }
                    }
                } else if(shtrFlag == HI) {        // shutter is being pressed
                    // check if shutter has been held long enough
                    if(TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) > DEBOUNCE_TIME) {
                        SHUTTER_PIN = LO;           // release shutter
                        shtrFlag = LO;              // shutter no longer pressed
                        picFlag = HI;
                    } else {
                        // wait for shutter trigger to debounce
                    }
                }
                break;
            case R_TRAVELLING:
                // check if moving
                if(movingFlag == LO) {
                    if(travDir == AWAY) {
                        nextPos = currPos + intSteps;
                        if(nextPos > awayPos) {
                            nextPos = awayPos;
                        }
                    } else {
                        nextPos = currPos - intSteps;
                        if(nextPos < homePos) {
                            nextPos = homePos;
                        }
                    }

                    // start to move
                    movingFlag = HI;
                    if(moveSingleStep(travDir)) {
                        currPos++;
                    }
                } else {
                    // check if at next position
                    if(currPos != nextPos) {
                        if(moveSingleStep(travDir)) {
                            currPos++;
                        }
                    } else {
                        movingFlag = LO;
                        runState = R_PROCESSING;
                    }
                }
                break;
            case R_PROCESSING:
                // check if done processing last pic
                if(TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) >= tProcess) {
                    // clear shutter flag and start time
                    shtrFlag = LO;
                    tShutterStart = tReset;
                    if(currPic <= totPics) {
                        // get ready to take next pic
                        runState = R_TAKING_PIC;
                    } else {
                        currPic = 1;
                        mainState = IDLE;
                    }
                } else {
                    // wait for last pic to process
                }
                break;
            default:
                // invalid run state option
                break;
        }
    }
}

void SM_init(void) {
    homePos = 0;
    awayPos = 6095;                 // 3' slide, 6095 steps end to end
    tProcess = 1;                   // init processing time as 1 ms
    currPic = 1;
    lastPic = currPic;
    totPics = 40;                   // init total pics as 40
    travDir = AWAY;                 // init travel direction as away from home
    intSteps = awayPos / totPics;   // calculate steps per interval
    startPos = homePos;
    currPos = homePos;              // guess current position is at home
    nextPos = homePos + intSteps;   // init next position variable
    shutterTime = 1;                // init shutter time as 1 ms (round up)
    mainState = STARTUP;            // current state is startup
    runState = R_TAKING_PIC;        // first run state is taking pic
    shtrFlag = LO;
    picFlag = LO;
    movingFlag = LO;

    UART1_init();
    inputs_init();
}