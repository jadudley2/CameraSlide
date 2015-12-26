/*
 * File:   statemachines.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:26 PM
 */

#include "statemachines.h"

extern timeObj tReset;
timeObj tShutterStart, tCurr;

extern unsigned int motor_stepsPerRev;
extern unsigned int motor_speed;
unsigned int processTime, shutterTime, lastPic, currPic, totPics, stepsPerInt,
        startPos, currPos, nextPos, homePos, awayPos, radius, length;

unsigned char mainState, runState, travDir, movingFlag, shutterFlag, picFlag,
        readCmdFlag, readValFlag, cmdBufferCount;

command cmdBuffer[CMD_BUFFER_SIZE];
command resetCmd = {MSG_BLANK, 0x00};
command tempCmd;

void SM_mainStateMachine(void) {
    while (1) {
        parseNextCommand();

        switch (mainState) {
            case STARTUP:
                mainState = IDLE;
            case IDLE:
            case FOCUS:
                // wait for command
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
                        sendCommand(MSG_MOVE_HOME, 0x00);
                    } else {
                        currPos = startPos;     // at start
                        mainState = FOCUS;      // go to focusing state
                        sendCommand(MSG_MOVE_START, 0x00);
                    }
                } else {    // keep moving
                    moveSingleStep(travDir);
                }
                break;
            case RUNNING:
                // clear all running flags and times
                shutterFlag = LO;
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
    sendCommand(MSG_TAKING_PIC, currPic);      // starts with taking pic
    
    while (mainState == RUNNING) {
        parseNextCommand();
        
        switch (runState) {
            case R_TAKING_PIC:
                if (shutterFlag == LO) {
                    if (picFlag == LO) {
                        SHUTTER_PIN = HI;               // initiate shutter
                        shutterFlag = HI;                  // shutter has been pressed
                        tShutterStart = TMR32_getTime();    // start shutter debounce
                    } else {
                        if (TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) > shutterTime) {
                            movingFlag = LO;            // get ready to move
                            currPic++;                  // pic has been snapped
                            picFlag = LO;
                            runState = R_TRAVELLING;    // move to next position
                            sendCommand(MSG_MOVING, 0x00);
                        } else {
                            // wait for pic to be taken
                        }
                    }
                } else if (shutterFlag == HI) {        // shutter is being pressed
                    // check if shutter has been held long enough
                    if (TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) > DEBOUNCE_TIME) {
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
                    if (travDir == AWAY) {
                        nextPos = currPos + stepsPerInt;
                        if (nextPos > awayPos) {
                            nextPos = awayPos;
                        }
                    } else {
                        nextPos = currPos - stepsPerInt;
                        if (nextPos < homePos) {
                            nextPos = homePos;
                        }
                    }

                    // start to move
                    movingFlag = HI;
                    if (moveSingleStep(travDir)) {
                        currPos++;
                    }
                } else {
                    // check if at next position
                    if (currPos != nextPos) {
                        if(moveSingleStep(travDir)) {
                            currPos++;
                        }
                    } else {
                        movingFlag = LO;
                        runState = R_PROCESSING;
                        sendCommand(MSG_PROCESSING, 0x00);
                    }
                }
                break;
            case R_PROCESSING:
                // check if done processing last pic
                if (TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) >= processTime) {
                    // clear shutter flag and start time
                    shutterFlag = LO;
                    tShutterStart = tReset;
                    if (currPic <= totPics) {
                        // get ready to take next pic
                        runState = R_TAKING_PIC;
                        sendCommand(MSG_TAKING_PIC, currPic);
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

void parseNextCommand(void) {
    // check if any commands in buffer
    if (checkCommandBuffer() != 0) {
        command cmd = readCommand(0);
        if (mainState != RUNNING) {
            switch (cmd.command) {
                case MSG_PICS:
                    totPics = cmd.value;
                    stepsPerInt = awayPos / totPics;
                    break;
                case MSG_SHUTTER:
                    shutterTime = cmd.value;
                    break;
                case MSG_PROCESS:
                    processTime = cmd.value;
                    break;
                case MSG_SPEED:
                    motor_speed = cmd.value;
                    break;
                case MSG_DIRECTION:
                    travDir = cmd.value;
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
                    mainState = MOVING_TO_START;
                    break;
                case MSG_ABORT:
                    movingFlag = LO;
                    mainState = IDLE;
                    break;
                 case MSG_MOVE_HOME:
                    mainState = MOVING_TO_HOME;
                    break;
                 case MSG_BLANK:
                    sendCommand(ACK_INVALID, cmd.command);
                    return;
                 default:
                    sendCommand(ACK_UNKNOWN, cmd.command);
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
                    sendCommand(ACK_INVALID, cmd.command);
                    return;
                 default:
                    sendCommand(ACK_UNKNOWN, cmd.command);
                    return;
            }
        }
        sendCommand(ACK_SUCCESS, 0x00);
    }
}

command readCommand(unsigned char pos) {
    if ((pos + 1) <= cmdBufferCount) {
        command cmd = cmdBuffer[pos];   // retrieve requested command
        // move commands remaining in buffer forward
        unsigned char j;
        for (j = pos; j < cmdBufferCount; j++) {
            cmdBuffer[pos] = cmdBuffer[pos + 1];
        }
        cmdBuffer[cmdBufferCount - 1] = resetCmd;
        cmdBufferCount--;       // decrement buffer count
        return cmd;
    } else {
        return resetCmd;        // return blank if pos outside of populated data
    }
}

void sendCommand(unsigned char msg, unsigned char val) {
    unsigned char bytes[4];
    bytes[0] = MSG_START_BYTE;
    bytes[1] = msg;
    bytes[2] = val;
    bytes[3] = MSG_END_BYTE;
    UART1_writeLine(bytes,sizeof(bytes));
}

unsigned char checkCommandBuffer(void) {
    unsigned char num = UART1_update();
    if (num > 0) {
        // read available byte from uart
        unsigned char chr = UART1_readLine(1)[0];
        // check if start command byte
        if (chr == MSG_START_BYTE) {
            if ((readCmdFlag|readValFlag) == HI) {
                // error
            } else {
                tempCmd = resetCmd;     // get ready to receive command
                readCmdFlag = HI;       // read command bytes first
            }
        // check if reading command
        } else if (readCmdFlag == HI) {
            tempCmd.command = chr;      // write byte to command
            readCmdFlag = LO;           // done reading command
            readValFlag = HI;           // reading val now
        // check if reading value
        } else if (readValFlag == HI) {
            tempCmd.value = chr;        // write byte to value
            readValFlag = LO;           // done reading val
        // check if end byte
        } else if (chr == MSG_END_BYTE) {
            if ((readCmdFlag|readValFlag) == HI) {
                // error
            } else {
                cmdBuffer[cmdBufferCount] = tempCmd;    // add cmd to buffer
            }
        }
    }
    return cmdBufferCount;
}

void SM_init(void) {
    currPic = 1;
    lastPic = currPic;
    totPics = TOTAL_PICS;           // init total pics as 40
    
    travDir = AWAY;                 // init travel direction as away from home
    awayPos = SLIDE_LENGTH * motor_stepsPerRev / (2 * PI * PULLEY_RADIUS);
    radius = PULLEY_RADIUS;
    length = SLIDE_LENGTH;
    stepsPerInt = awayPos / totPics;   // calculate steps per interval

    homePos = 0;
    startPos = homePos;
    currPos = homePos;              // guess current position is at home
    nextPos = homePos + stepsPerInt;   // init next position variable
    
    processTime = PROCESS_TIME;        // init processing time
    shutterTime = SHUTTER_TIME;     // init shutter time as 1 ms (round up)

    mainState = STARTUP;            // current state is startup
    runState = R_TAKING_PIC;        // first run state is taking pic
    shutterFlag = LO;
    picFlag = LO;
    movingFlag = LO;

    inputs_init();
}