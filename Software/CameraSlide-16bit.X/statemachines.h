/* 
 * File:   statemachines.h
 * Author: James Dudley
 *
 * Created on May 23, 2015, 11:32 AM
 */

#ifndef STATEMACHINES_H
#define	STATEMACHINES_H

#include "constants.h"
#include "TMR32.h"
#include "LCD.h"
#include "inputs.h"
#include "motor.h"

/* states */
#define STARTUP             0
#define IDLE                1
#define SETUP               2
#define FOCUS               3
#define MOVING_TO_HOME      4
#define MOVING_TO_START     5
#define RUNNING             6

#define R_TAKING_PIC        61
#define R_TRAVELLING        62
#define R_PROCESSING        63

/* screens */
#define IDLE_SCREEN         0
#define SET_PICS            1
#define SET_SHUTTER         2
#define SET_DIRECTION       3
#define FOCUS_SCREEN        4
#define MOVE_SCREEN         5
#define RUN_SCREEN          6

#define MIN_PICS            1
#define MAX_PICS            AWAY_POS

#define IDLE_SCREEN_1       "Total Pics: %d"
#define IDLE_SCREEN_2       "Shutter: "
#define SET_PICS_1          "Set Total Pics"
#define SET_SHUTTER_1       "Set Shutter"
#define SET_DIRECTION_1     "Set Direction"
#define FOCUS_SCREEN_1      "Focus Camera"
#define MOVE_SCREEN_1       "Moving to Home"
#define MOVE_SCREEN_2       "Moving to Start"
#define RUN_SCREEN_1        "Running..."
#define RUN_SCREEN_2        "Pic %d of %d"

void SM_mainStateMachine(void);

void SM_runStateMachine(void);

void SM_updateDisplay(unsigned char scr);

void SM_init(void);

#endif	/* STATEMACHINES_H */

