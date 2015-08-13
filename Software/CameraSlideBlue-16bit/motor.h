/* 
 * File:   motor.h
 * Author: jadudley
 *
 * Created on April 2, 2015, 8:55 PM
 */

#ifndef MOTOR_H
#define	MOTOR_H

#include "constants.h"
#include "TMR32.h"

#define STEP_INTERVAL           10         // milliseconds, min btwn steps
#define STEPS_PER_REV           200

/*
 * moveSingleStep
 * advances stepper motor a single step in prescribed direction
 * takes: unsigned char representing direction to turn stepper
 * returns: nothing
 */
unsigned char moveSingleStep(unsigned char dir);

void motor_init(void);

#endif	/* MOTOR_H */

