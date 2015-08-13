/*
 * File:   LCD_lib.c
 * Author: jadudley
 *
 * Created on February 15, 2015
 */

#include "LCD.h"

timeObj LCD_lastCall;
unsigned char cursPos;

void LCD_init() {
    LCD_RS_PIN = LO;
    LCD_EN_PIN = LO;

    LCD_lastCall = TMR32_getTime();
    while(TMR32_timeDiff(LCD_lastCall, TMR32_getTime(), MSEC) < LCD_POWERUP_TIME) {
        // wait for powerup to complete, >15ms
    }

    LCD_command(0x33);          // wake up x2
    LCD_command(0x32);          // wake up, 4-bit
    LCD_command(0x28);          // 4-bit, 2 line
    LCD_command(0x08);
    LCD_command(LCD_CLEAR);
    LCD_command(0x06);
    LCD_command(0x0C);          

    LCD_lastCall = TMR32_getTime();
    while(TMR32_timeDiff(LCD_lastCall, TMR32_getTime(), MSEC) < LCD_STARTUP_TIME) {
        // wait for LCD to finish startup
    }
    cursPos = 0;
}

void LCD_print(char character, unsigned char adv) {
    while(TMR32_timeDiff(TMR32_getTime(), LCD_lastCall, MSEC) < LCD_DELAY_TIME) {
        // makes sure enough time has passed since last call
    }

    // pulse data input pin, send left 4 bits
    LCD_RS_PIN = 1;
    LCD_nibble(character>>4);
    LCD_RS_PIN = 0;

    // pulse data input pin, send right 4 bits
    LCD_RS_PIN = 1;
    LCD_nibble(character);
    LCD_RS_PIN = 0;

    // check whether or not to advance the cursor
    if(adv == OFF) {
        LCD_command(LCD_SET_POS | cursPos);
    } else {
        cursPos++;
    }
    LCD_lastCall = TMR32_getTime();
}

void LCD_write(char *str, unsigned char length) {
    int i;
    for(i = 0; i < length; i++) {
        if(str[i] != '\0') {
            LCD_print(str[i], ON);
        }
    }
}

void LCD_command(unsigned char commd) {
    while(TMR32_timeDiff(TMR32_getTime(), LCD_lastCall, MSEC) < LCD_DELAY_TIME) {
        //make sure enough time has elapsed since last call
    }

    // send left 4 bits
    LCD_nibble(commd>>4);

    // send right 4 bits
    LCD_nibble(commd);

    LCD_lastCall = TMR32_getTime();
}

void LCD_nibble(unsigned char commd) {
    // send 4 bits
    LCD_D4_PIN = (commd & 0b0001);
    LCD_D5_PIN = (commd & 0b0010) >> 1;
    LCD_D6_PIN = (commd & 0b0100) >> 2;
    LCD_D7_PIN = (commd & 0b1000) >> 3;

    // pulse enable pin
    LCD_EN_PIN = 1;
    timeObj tNib = TMR32_getTime();
    while(TMR32_timeDiff(tNib, TMR32_getTime(), USEC) < 1) {
        // wait for enable pin pulse (>300ns)
    }
    LCD_EN_PIN = 0;
}