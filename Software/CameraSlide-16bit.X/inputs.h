/* 
 * File:   inputs.h
 * Author: James Dudley
 *
 * Created on May 23, 2015, 12:16 PM
 */

#ifndef INPUTS_H
#define	INPUTS_H

#include "constants.h"
#include "TMR32.h"

#define DEBOUNCE_TIME               40      // milliseconds
#define LONG_PRESS_TIME             600     // milliseconds
#define NUM_AGGREGATE_SAMPLES       10
#define AGGREGATE_THRESHOLD         8

/* inputs */
#define NUM_INPUTS          8
#define NUM_BUTTONS         6
#define NUM_SWITCHES        2
#define NO_INPUT            NUM_INPUTS
#define START_BUTTON        0
#define START_BUTTON_LONG   1
#define CENTER_BUTTON       2
#define CENTER_BUTTON_LONG  3
#define UP_BUTTON           4
#define DOWN_BUTTON         5
#define HOME_SWITCH         HOME
#define AWAY_SWITCH         AWAY

unsigned char checkButtons(void);

unsigned char checkSingleInput(unsigned char input);

unsigned char checkInputState(unsigned char input);

void inputs_init(void);

#endif	/* INPUTS_H */

