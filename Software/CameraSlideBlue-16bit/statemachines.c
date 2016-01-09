/*
 * File:   statemachines.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:26 PM
 */

#include "statemachines.h"

extern timeObj tReset;
timeObj tShutterStart, tCurr;

extern unsigned char UART1_cmdFlag;

extern unsigned int motor_stepsPerRev;
extern unsigned int motor_speed;
unsigned int processTime, shutterTime, lastPic, currPic, totPics, stepsPerInt,
        startPos, currPos, nextPos, homePos, awayPos, radius, length;

unsigned char mainState, runState, travDir, movingFlag, shutterFlag, picFlag,
        readCmdFlag, readValFlag;

timeObj tStart, tNow;

void SM_mainStateMachine(void) {
    motor_init(MOTOR_SPEED, MOTOR_STEPS_PER_REV);
    SM_init();

    while (1) {
        SM_heartbeat();
        SM_parseNextCommand();

        if (ADC_rawValue() < VOLTAGE_THRESHOLD) {
            PWR_LED_PIN = LO;
        } else {
            PWR_LED_PIN = HI;
        }
        
        switch (mainState) {
            case STARTUP:
                mainState = IDLE;
            case IDLE:
            case FOCUS:     // wait for next command
                break;      
            case MOVING_TO_HOME:
            case MOVING_TO_START:
                movingFlag = HI;
                // check end limit switches
                if (checkInputState(travDir) == HI) {
                    movingFlag = LO;            // no longer moving
                    if (mainState == MOVING_TO_HOME) {
                        currPos = homePos;      // at home
                        mainState = IDLE;       // go to idle state
                        UART1_sendCommand(MSG_MOVE_HOME, 0);
                    } else {
                        currPos = startPos;     // at start
                        mainState = FOCUS;      // go to focusing state
                        UART1_sendCommand(MSG_MOVE_START, 0);
                    }
                } else {    // keep moving
                    moveSingleStep(travDir);
                }
                break;
            case RUNNING:
                // run the running state machine
                SM_runStateMachine();
                SYS_LED_PIN = LO;
                break;
            default:
                // invalid main state option
                break;
        }
    }
}

void SM_runStateMachine(void) {
    // clear all running flags and times
    shutterFlag = LO;
    movingFlag = LO;
    tShutterStart = tReset;
    currPic = 1;

    // intialize running state to taking pic
    runState = R_TAKING_PIC;
    
    while (mainState == RUNNING) {
        SM_heartbeat();
        SM_parseNextCommand();

        if (ADC_rawValue() < VOLTAGE_THRESHOLD) {
            PWR_LED_PIN = LO;
        } else {
            PWR_LED_PIN = HI;
        }
        
        switch (runState) {
            case R_TAKING_PIC:
                if (shutterFlag == LO) {
                    if (picFlag == LO) {
                        UART1_sendCommand(MSG_TAKING_PIC,currPic);
                        SHUTTER_PIN = HI;               // initiate shutter
                        shutterFlag = HI;                  // shutter has been pressed
                        tShutterStart = TMR32_getTime();    // start shutter debounce
                    } else {
                        if (TMR32_timeDiff(TMR32_getTime(),tShutterStart,MSEC) >= shutterTime) {
                            movingFlag = LO;            // get ready to move
                            currPic++;                  // pic has been snapped
                            picFlag = LO;
                            runState = R_TRAVELLING;    // move to next position
                        } else {
                            // wait for pic to be taken
                        }
                    }
                } else if (shutterFlag == HI) {        // shutter is being pressed
                    // check if shutter has been held long enough
                    if (TMR32_timeDiff(TMR32_getTime(),tShutterStart, MSEC) > DEBOUNCE_TIME) {
                        SHUTTER_PIN = LO;           // release shutter
                        shutterFlag = LO;              // shutter no longer pressed
                        picFlag = HI;
                    } else {
                        // wait for shutter trigger to debounce
                    }
                }
                break;
            case R_TRAVELLING:
                // check if moving
                if (movingFlag == LO) {
                    UART1_sendCommand(MSG_MOVING,travDir);
                    if (travDir == AWAY) {
                        nextPos = currPos + stepsPerInt;
                        if (nextPos > awayPos) {
                            nextPos = awayPos;
                        }
                    } else {
                        nextPos = currPos - stepsPerInt;
                        UART1_sendCommand(currPos,nextPos);
                        if (nextPos < homePos) {
                            nextPos = homePos;
                        }
                    }

                    // start to move
                    movingFlag = HI;
                    if (checkInputState(travDir) != HI) {
                        if (moveSingleStep(travDir)) {
                            if (travDir == AWAY) {
                                currPos++;
                            } else {
                                currPos--;
                            }
                        }
                    } else {
                        mainState = IDLE;
                    }
                } else {
                    //TODO: check limit switch to see if we're at the end
                    // check if at next position
                    if (currPos != nextPos) {
                        if(moveSingleStep(travDir)) {
                            currPos++;
                        }
                    } else {
                        movingFlag = LO;
                        runState = R_PROCESSING;
                        UART1_sendCommand(MSG_PROCESSING, 0);
                    }
                }
                break;
            case R_PROCESSING:
                // check if done processing last pic
                if (TMR32_timeDiff(TMR32_getTime(),tShutterStart,MSEC) >= processTime) {
                    // clear shutter flag and start time
                    shutterFlag = LO;
                    tShutterStart = tReset;
                    if (currPic <= totPics) {
                        // get ready to take next pic
                        runState = R_TAKING_PIC;
                        UART1_sendCommand(MSG_TAKING_PIC, currPic);
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

void SM_parseNextCommand(void) {
    // check if any commands in buffer
    if (UART1_cmdFlag != 0) {
        command cmd = UART1_readCommand();
        if (mainState != RUNNING) {
            switch (cmd.command) {
                case MSG_PICS:
                    totPics = cmd.value;
                    stepsPerInt = awayPos / totPics;
                    UART1_sendCommand(ACK_SUCCESS,MSG_PICS);
                    break;
                case MSG_SHUTTER:
                    shutterTime = cmd.value;
                    UART1_sendCommand(ACK_SUCCESS,MSG_SHUTTER);
                    break;
                case MSG_PROCESS:
                    processTime = cmd.value;
                    break;
                case MSG_SPEED:
                    motor_speed = cmd.value;
                    break;
                case MSG_DIRECTION:
                    travDir = cmd.value;
                    switch (travDir) {
                        case HOME:
                            startPos = awayPos;
                            break;
                        case AWAY:
                            startPos = homePos;
                            break;
                        default:
                            UART1_sendCommand(ACK_FAILURE,0);
                    }
                    break;
                case MSG_SLIDE_LENGTH:
                    length = cmd.value;
                    awayPos = length * motor_stepsPerRev / (2 * PI * radius);
                    stepsPerInt = awayPos / totPics;
                    break;
                case MSG_RADIUS:
                    radius = cmd.value;
                    awayPos = length * motor_stepsPerRev / (2 * PI * radius);
                    stepsPerInt = awayPos / totPics;
                    break;
                case MSG_ANG_STEP:
                    motor_stepsPerRev = cmd.value;
                    awayPos = length * motor_stepsPerRev / (2 * PI * radius);
                    stepsPerInt = awayPos / totPics;
                    break;
                case MSG_RUN:
                    if (mainState == IDLE) {
                        mainState = MOVING_TO_START;
                    } else {
                        mainState = RUNNING;
                    }
                    break;
                case MSG_ABORT:
                    movingFlag = LO;
                    mainState = IDLE;
                    break;
                 case MSG_MOVE_HOME:
                    mainState = MOVING_TO_HOME;
                    break;
                 case MSG_BLANK:
                    UART1_sendCommand(ACK_INVALID,cmd.command);
                    return;
                 default:
                    UART1_sendCommand(ACK_UNKNOWN,cmd.command);
                    return;
            }
        } else {
            // only certain commands are valid for running state
            switch (cmd.command) {
                case MSG_ABORT:
                    currPic = 1;
                    mainState = IDLE;
                    break;
                 case MSG_BLANK:
                    UART1_sendCommand(ACK_INVALID,cmd.command);
                    return;
                 default:
                    UART1_sendCommand(ACK_UNKNOWN,cmd.command);
                    return;
            }
        }
        UART1_sendCommand(ACK_SUCCESS,cmd.command);
    }
}

void SM_init(void) {
    currPic = 1;
    lastPic = currPic;
    totPics = TOTAL_PICS;           // init total pics
    travDir = AWAY;                 // init travel direction as away from home
    awayPos = SLIDE_LENGTH * motor_stepsPerRev / (2 * PI * PULLEY_RADIUS);
    radius = PULLEY_RADIUS;
    length = SLIDE_LENGTH;
    stepsPerInt = awayPos / totPics;   // calculate steps per interval

    homePos = 0;
    startPos = homePos;
    currPos = homePos;              // guess current position is at home
    nextPos = homePos + stepsPerInt;   // init next position variable
    
    processTime = PROCESS_TIME;     // init processing time
    shutterTime = SHUTTER_TIME;     // init shutter time

    mainState = STARTUP;            // current state is startup
    runState = R_TAKING_PIC;        // first run state is taking pic
    shutterFlag = LO;
    picFlag = LO;
    movingFlag = LO;

    tStart = TMR32_getTime();
    tNow = tStart;

    SYS_LED_PIN = LO;
    PWR_LED_PIN = LO;

    inputs_init();
}

void SM_heartbeat(void) {
    // heartbeat
    tNow = TMR32_getTime();
    if (TMR32_timeDiff(tStart,tNow,MSEC) >= HB_INTERVAL) {
        HB_LED_PIN = ~HB_LED_PIN;
//        UART1_sendCommand(MSG_HEARTBEAT,0);
        // TODO: get HB response (in parseNextCommand?)
        tStart = tNow;
    }
}