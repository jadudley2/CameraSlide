/*
 * File:   statemachines.c
 * Author: James Dudley
 *
 * Created on May 23, 2015, 11:32 AM
 */

#include "statemachines.h"

extern timeObj tReset;
timeObj tShutterStart, tPrevPress, tCurr;

unsigned int tProcess, lastPic, currPic, totPics, intSteps, startPos, currPos,
        nextPos, iShtr;

unsigned char travDir, mainState, runState, setScr, currScr, movingFlag,
        shtrFlag, picFlag, i, btnFlag1, btnFlag2;

char shutterSpeeds[][SHUTTER_STR_LENGTH + 1] = {
 "1/8000\0","1/6400\0","1/5000\0","1/4000\0","1/3200\0","1/2500\0","1/2000\0","1/1600","1/1250\0",
"1/1000\0","1/800\0","1/640\0","1/500\0","1/400\0","1/320\0","1/250\0",
"1/200\0","1/160\0","1/125\0","1/100\0","1/80\0","1/60\0","1/50\0","1/40\0",
"1/30\0","1/25\0","1/20\0","1/15\0","1/13\0","1/10\0","1/8\0","1/6\0","1/5\0",
"1/4\0","0.3\0","0.4\0","0.5\0","0.6\0","0.8\0","1.0\0","1.3\0","1.6\0","2.0\0",
"2.5\0","3.2\0","4.0\0","5.0\0","6.0\0","8.0\0","10.0\0","13.0\0","15.0\0",
"20.0\0","25.0\0","30.0\0"
};
unsigned int shutterTimes[] = {
1,1,1,1,1,1,1,1,1,
2,2,2,3,3,4,5,
6,8,9,11,15,18,21,26,
35,42,52,68,78,105,130,175,210,
260,310,410,510,610,810,1010,1310,1610,2010,
2510,3210,4010,5010,6010,8010,10020,13020,15020,
20020,25020,3020
};
char dirStr[][DIR_STR_LENGTH + 1] = {"HOME\0", "AWAY\0"};
char disp[LCD_WIDTH];

void SM_mainStateMachine(void) {
    while(ON) {
        switch(mainState) {
            case STARTUP:
                mainState = IDLE;
                break;
            case IDLE:
                SM_updateDisplay(IDLE_SCREEN);

                switch(checkButtons()) {
                    case START_BUTTON:
                        movingFlag = LO;
                        mainState = MOVING_TO_START;
                        break;
                    case START_BUTTON_LONG:
                        movingFlag = LO;
                        mainState = MOVING_TO_HOME;
                        break;
                    case CENTER_BUTTON:
                        setScr = SET_PICS;
                        mainState = SETUP;
                        break;
                    default:
                        // no relevant button pressed
                        break;
                }
                break;
            case SETUP:
                SM_updateDisplay(setScr);
                
                switch(checkButtons()) {
                    case START_BUTTON_LONG:
                        mainState = IDLE;
                        break;
                    case CENTER_BUTTON:
                        switch(setScr) {
                            case SET_PICS:
                                setScr = SET_SHUTTER;
                                break;
                            case SET_SHUTTER:
                                setScr = SET_DIRECTION;
                                break;
                            case SET_DIRECTION:
                                setScr = SET_PICS;
                                break;
                            default:
                                // invalid setup screen
                                break;
                        }
                        break;
                    case START_BUTTON:
                        movingFlag = LO;
                        mainState = MOVING_TO_START;
                        break;
                    default:
                        // no relevant button pressed
                        break;
                }
                break;
            case FOCUS:
                SM_updateDisplay(FOCUS_SCREEN);
                switch(checkButtons()) {
                    case START_BUTTON_LONG:
                        mainState = IDLE;
                        break;
                    case CENTER_BUTTON:
                        setScr = SET_PICS;
                        mainState = SETUP;
                        break;
                    case START_BUTTON:
                        mainState = RUNNING;
                        break;
                    default:
                        // no relevant buttons pressed
                        break;
                }
                break;
            case MOVING_TO_HOME:
                // OR
            case MOVING_TO_START:
                SM_updateDisplay(MOVE_SCREEN);
                
                // check buttons
                switch(checkButtons()) {
                    case START_BUTTON:
                        // go to idle state
                        movingFlag = LO;
                        mainState = IDLE;
                        return;
                        break;
                    case CENTER_BUTTON:
                        // go to set state
                        movingFlag = LO;
                        mainState = SETUP;
                        return;
                        break;
                    default:
                        // invalid button
                        break;
                }

                // start moving
                movingFlag = HI;
                // check end limit switches
                unsigned char s = travDir;
                if(checkInputState(s) == HI) {
                    movingFlag = LO;            // no longer moving
                    if(mainState == MOVING_TO_HOME) {
                        currPos = HOME_POS;     // at home
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
        // check if start button has been held
        if(checkButtons() == START_BUTTON_LONG) {
            // reset running flags, shutter start time, pics taken, etc.
            shtrFlag = LO;
            movingFlag = LO;
            tShutterStart = tReset;
            currPic = 1;

            // reset running state, set main state to IDLE
            runState = R_TAKING_PIC;
            mainState = IDLE;
            return;
        }

        SM_updateDisplay(RUN_SCREEN);

        switch(runState) {
            case R_TAKING_PIC:
                if(shtrFlag == LO) {
                    if(picFlag == LO) {
                        SHUTTER_PIN = HI;               // initiate shutter
                        shtrFlag = HI;                  // shutter has been pressed
                        tShutterStart = TMR32_getTime();    // start shutter debounce
                    } else {
                        if(TMR32_timeDiff(TMR32_getTime(), tShutterStart, MSEC) > shutterTimes[iShtr]) {
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
                        if(nextPos > AWAY_POS) {
                            nextPos = AWAY_POS;
                        }
                    } else {
                        nextPos = currPos - intSteps;
                        if(nextPos < HOME_POS) {
                            nextPos = HOME_POS;
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

void SM_updateDisplay(unsigned char scr) {
    unsigned char changeFlag = LO;
    switch(scr) {
        case IDLE_SCREEN:
            if(currScr != IDLE_SCREEN) {
                LCD_command(LCD_CLEAR);
                // write first line
                sprintf(disp, IDLE_SCREEN_1, totPics);
                LCD_write(disp, sizeof(disp));
                // write second line
                LCD_command(LCD_NEXT_LINE);
                char str[] = IDLE_SCREEN_2;
                LCD_write(str, sizeof(str));
                LCD_write(shutterSpeeds[iShtr], SHUTTER_STR_LENGTH);
                currScr = IDLE_SCREEN;
            }
            break;
        case SET_PICS:
            if(currScr != SET_PICS) {
                LCD_command(LCD_CLEAR);
                // write first line
                char str[] = SET_PICS_1;
                LCD_write(str, sizeof(str));
                changeFlag = HI;
                currScr = SET_PICS;
            }

            //TODO: optimize up/down button release as function
            // deal with up button press/release
            if(checkSingleInput(UP_BUTTON) == HI) {
                if(totPics < MAX_PICS) {
                    tCurr = TMR32_getTime();
                    // check if first push for a while
                    if((btnFlag1 == LO) && (btnFlag2 == LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME) {
                            btnFlag1 = HI;
                            changeFlag = HI;
                            totPics++;
                            tPrevPress = tCurr;
                        }
                        // check if held for second press
                    } else if((btnFlag1 = HI) && (btnFlag2 = LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME * 2) {
                            btnFlag2 = HI;
                            changeFlag = HI;
                            totPics++;
                            tPrevPress = tCurr;
                        }
                        // else, still held
                    } else {
                        changeFlag = HI;
                        totPics++;
                        tPrevPress = tCurr;
                    }
                }
            } else {
                btnFlag1 = LO;
                btnFlag2 = LO;
            }

            // deal with down button press/release
            if(checkSingleInput(DOWN_BUTTON) == HI) {
                if(totPics > MIN_PICS) {
                    tCurr = TMR32_getTime();
                    // check if first push for a while
                    if((btnFlag1 == LO) && (btnFlag2 == LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME) {
                            btnFlag1 = HI;
                            changeFlag = HI;
                            totPics--;
                            tPrevPress = tCurr;
                        }
                        // check if held for second press
                    } else if((btnFlag1 = HI) && (btnFlag2 = LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME * 2) {
                            btnFlag2 = HI;
                            changeFlag = HI;
                            totPics--;
                            tPrevPress = tCurr;
                        }
                        // else, still held
                    } else {
                        changeFlag = HI;
                        totPics--;
                        tPrevPress = tCurr;
                    }
                }
            } else {
                btnFlag1 = LO;
                btnFlag2 = LO;
            }

            if(changeFlag == HI) {
                for(i = 0; i < sizeof(disp); i++) disp[i] = ' ';
                // write second line
                LCD_command(LCD_NEXT_LINE);
                sprintf(disp, "%d", totPics);
                LCD_write(disp, sizeof(disp));
                changeFlag = LO;
            }
            break;
        case SET_SHUTTER:
            if(currScr != SET_SHUTTER) {
                LCD_command(LCD_CLEAR);
                // write first line
                char str[] = SET_SHUTTER_1;
                LCD_write(str, sizeof(str));
                // write second line
                LCD_command(LCD_NEXT_LINE);
                LCD_write(shutterSpeeds[iShtr], SHUTTER_STR_LENGTH);
                currScr = SET_SHUTTER;
            }

            // deal with up button press/release
            if(checkSingleInput(UP_BUTTON) == HI) {
                if(iShtr < (NUM_SHUTTER_SPEEDS - 1)) {
                    tCurr = TMR32_getTime();
                    // check if first push for a while
                    if((btnFlag1 == LO) && (btnFlag2 == LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME) {
                            btnFlag1 = HI;
                            changeFlag = HI;
                            iShtr++;
                            tPrevPress = tCurr;
                        }
                        // check if held for second press
                    } else if((btnFlag1 = HI) && (btnFlag2 = LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME * 2) {
                            btnFlag2 = HI;
                            changeFlag = HI;
                            iShtr++;
                            tPrevPress = tCurr;
                        }
                        // else, still held
                    } else {
                        changeFlag = HI;
                        iShtr++;
                        tPrevPress = tCurr;
                    }
                }
            } else {
                btnFlag1 = LO;
                btnFlag2 = LO;
            }

            // deal with down button press/release
            if(checkSingleInput(DOWN_BUTTON) == HI) {
                if(iShtr > 0) {
                    tCurr = TMR32_getTime();
                    // check if first push for a while
                    if((btnFlag1 == LO) && (btnFlag2 == LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME) {
                            btnFlag1 = HI;
                            changeFlag = HI;
                            iShtr--;
                            tPrevPress = tCurr;
                        }
                        // check if held for second press
                    } else if((btnFlag1 = HI) && (btnFlag2 = LO)) {
                        if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME * 2) {
                            btnFlag2 = HI;
                            changeFlag = HI;
                            iShtr--;
                            tPrevPress = tCurr;
                        }
                        // else, still held
                    } else {
                        changeFlag = HI;
                        iShtr--;
                        tPrevPress = tCurr;
                    }
                }
            } else {
                btnFlag1 = LO;
                btnFlag2 = LO;
            }

            if(changeFlag == HI) {
                // write second line
                LCD_command(LCD_NEXT_LINE);
                for(i = 0; i < sizeof(disp); i++) disp[i] = ' ';
                snprintf(disp, sizeof(disp), "%s", shutterSpeeds[iShtr]);
                LCD_write(disp, sizeof(disp));
                changeFlag = LO;
            }
            break;
        case SET_DIRECTION:
            if(currScr != SET_DIRECTION) {
                LCD_command(LCD_CLEAR);
                // write first line
                char str[] = SET_DIRECTION_1;
                LCD_write(str, sizeof(str));
                // write second line
                LCD_command(LCD_NEXT_LINE);
                LCD_write(dirStr[travDir - HOME], DIR_STR_LENGTH);
                currScr = SET_DIRECTION;
            }

            if((checkSingleInput(UP_BUTTON) == HI) || (checkSingleInput(DOWN_BUTTON) == HI)) {
                if(btnFlag1 == LO) {
                    tCurr = TMR32_getTime();
                    if(TMR32_timeDiff(tCurr, tPrevPress, MSEC) > DEBOUNCE_TIME) {
                        changeFlag = HI;
                        if(travDir == HOME) {
                            travDir = AWAY;
                        } else {
                            travDir = HOME;
                        }
                        tPrevPress = tCurr;
                        btnFlag1 = HI;
                    }
                }
            } else {
                btnFlag1 = LO;
            }

            if(changeFlag == HI) {
                // write second line
                LCD_command(LCD_NEXT_LINE);
                LCD_write(dirStr[travDir - HOME], DIR_STR_LENGTH);
                changeFlag = LO;
            }
            break;
        case FOCUS_SCREEN:
            if(currScr != FOCUS_SCREEN) {
                LCD_command(LCD_CLEAR);
                // write first line
                char str[] = FOCUS_SCREEN_1;
                LCD_write(str, sizeof(str));
                currScr = FOCUS_SCREEN;
            }
            break;
        case MOVE_SCREEN:
            if(currScr != MOVE_SCREEN) {
                LCD_command(LCD_CLEAR);
                // write first line
                if(mainState == MOVING_TO_HOME) {
                    char str[] = MOVE_SCREEN_1;
                    LCD_write(str, sizeof(str));
                } else {
                    char str[] = MOVE_SCREEN_2;
                    LCD_write(str, sizeof(str));
                }
                currScr = MOVE_SCREEN;
            }
            break;
        case RUN_SCREEN:
            if(currScr != RUN_SCREEN) {
                LCD_command(LCD_CLEAR);
                // write first line
                char str[] = RUN_SCREEN_1;
                LCD_write(str, sizeof(str));
                // write second line
                LCD_command(LCD_NEXT_LINE);
                for(i = 0; i < sizeof(disp); i++) disp[i] = ' ';
                sprintf(disp, RUN_SCREEN_2, currPic, totPics);
                LCD_write(disp, sizeof(disp));
                currScr = RUN_SCREEN;
            }
            if(currPic != lastPic) {
                lastPic = currPic;
                // write second line
                LCD_command(LCD_NEXT_LINE);
                for(i = 0; i < sizeof(disp); i++) disp[i] = ' ';
                sprintf(disp, RUN_SCREEN_2, currPic, totPics);
                LCD_write(disp, sizeof(disp));
            }
            break;
        default:
            // invalid screen option
            break;
    }
}

void SM_init(void) {
    tProcess = 1;                   // init processing time as 1 ms
    currPic = 1;
    lastPic = currPic;
    totPics = 40;                   // init total pics as 40
    travDir = AWAY;                 // init travel direction as away from home
    intSteps = AWAY_POS / totPics;  // calculate steps per interval
    startPos = HOME_POS;
    currPos = HOME_POS;             // guess current position is at home
    nextPos = HOME_POS + intSteps;  // init next position variable
    iShtr = NUM_SHUTTER_SPEEDS / 2; // init shutter index as middle of range
    mainState = STARTUP;            // current state is startup
    runState = R_TAKING_PIC;        // first run state is taking pic
    setScr = SET_PICS;
    currScr = RUN_SCREEN;
    shtrFlag = LO;
    picFlag = LO;
    movingFlag = LO;
    btnFlag1 = LO;
    btnFlag2 = LO;
    tPrevPress = tReset;

    inputs_init();
    LCD_init();
}