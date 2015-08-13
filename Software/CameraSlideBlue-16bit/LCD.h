/*
 * File:   LCD_lib.h
 * Author: jadudley
 *
 * Created on February 15, 2015
 *
 */

#ifndef LCD_LIB_H
#define	LCD_LIB_H

#include "constants.h"
#include "TMR32.h"

#define LCD_POWERUP_TIME        20          // milliseconds
#define LCD_STARTUP_TIME        200         // milliseconds
#define LCD_DELAY_TIME          10          // milliseconds

/* COMMANDS */
#define LCD_NEXT_LINE           0b11000000
#define LCD_RETURN_HOME         0b00000010
#define LCD_CLEAR               0b00000001
#define LCD_FUNC_SET            0b00100000
#define LCD_DISP                0b00001000
#define LCD_4_BIT               0b00000000
#define LCD_8_BIT               0b00010000
#define LCD_1_LINE              0b00000000
#define LCD_2_LINE              0b00001000
#define LCD_DOT_MATRIX          0b00000000
#define LCD_DISP_ON             0b00000100
#define LCD_DISP_OFF            0b00000000
#define LCD_CURSOR_ON           0b00000010
#define LCD_CURSOR_OFF          0b00000000
#define LCD_BLINK_ON            0b00000001
#define LCD_BLINK_OFF           0b00000000
#define LCD_MOVE_CURSOR         0b00001000
#define LCD_SHIFT_DISPLAY       0b00001100
#define LCD_TO_LEFT             0b00000000
#define LCD_TO_RIGHT            0b00000100
#define LCD_NO_CMD              0b00000000
#define LCD_SET_POS             0b10000000

/*
 * Initializes LCD display
 */
void LCD_init();

/*
 * Prints single character to the display.
 * data - single byte of data to be printed
 * adv - ON or OFF - whether to advance cursor after write
 */
void LCD_print(char data, unsigned char adv);


/*
 * Writes a series of characters to the display
 * str - pointer to beginning of string to be written
 * length - length of string to be written
 */
void LCD_write(char *str, unsigned char length);

/*
 * Sends a command to the display.
 * cmmd - command to be sent, format: ( COMMAND | SETTING 1 | ... | SETTING n )
 */
void LCD_command(unsigned char commd);

void LCD_nibble(unsigned char commd);

#endif	/* LCD_LIB_H */

