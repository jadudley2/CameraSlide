/* 
 * File:   inputs.h
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:11 PM
 */

#ifndef INPUTS_H
#define	INPUTS_H

#include "constants.h"
#include "TMR32.h"

#define NUM_AGGREGATE_SAMPLES       10
#define AGGREGATE_THRESHOLD         8

/* inputs */
#define NUM_INPUTS          2
#define NUM_SWITCHES        2
#define NO_INPUT            NUM_INPUTS
#define HOME_SWITCH         HOME
#define AWAY_SWITCH         AWAY

unsigned char checkInputState(unsigned char input);

void inputs_init(void);

#endif	/* INPUTS_H */

