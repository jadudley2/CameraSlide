/* 
 * File:   statemachines.h
 * Author: James Dudley
 *
 * Created on August 12, 11:47 PM
 */

#ifndef STATEMACHINES_H
#define	STATEMACHINES_H

#include "constants.h"
#include "TMR32.h"
#include "UART1.h"
#include "inputs.h"
#include "motor.h"

/* states */
#define STARTUP             0
#define IDLE                1
#define FOCUS               2
#define MOVING_TO_HOME      3
#define MOVING_TO_START     4
#define RUNNING             5

#define R_TAKING_PIC        51
#define R_TRAVELLING        52
#define R_PROCESSING        53

void SM_mainStateMachine(void);

void SM_runStateMachine(void);

void SM_init(void);

#endif	/* STATEMACHINES_H */

