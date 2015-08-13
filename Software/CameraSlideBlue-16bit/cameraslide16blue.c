/* 
 * File:   cameraslide16.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:47 PM
 */

#include "cameraslide16blue.h"

#include "UART1.h"

int main() {

    main_init();

    // test UART
    UART1_init();
    timeObj tm1 = TMR32_getTime();
    timeObj tm2 = tm1;
    unsigned char updateLen = 0;
    while(ON) {
        while(TMR32_timeDiff(tm1,tm2,MSEC) < 500) {
            updateLen = UART1_update();
            tm2 = TMR32_getTime();
        }
        unsigned char line[] = "hello world\0";
        UART1_writeLine(line);

        if (updateLen > 0) {
            unsigned char* read = UART1_readLine(updateLen);
            printf(read);
        }


        //SM_mainStateMachine();
    }

    return (EXIT_SUCCESS);
}

void main_init(void) {

//    CM2CONbits.CON = 0;         // disable comp2 to use as digital I/O
//    SPI2STATbits.SPIEN = 0;     // disable spi2 to use as digital I/O
//    SPI2CON1bits.DISSDO = 1;    // disable sdo2 to use as digital I/O
//    SPI2CON1bits.DISSCK = 1;    // disable sck2 to use as digital I/O
//    I2C1CONbits.I2CEN = 0;      // disable I2C to use as digital I/O
    
    ANSA = 0x000;               // init all port A as digital
    TRISA = 0x000;              // init all port A as outputs
    LATA = 0x000;               // init all port A as lo
    ANSB = 0x000;               // init all port B as digital
    TRISB = 0x000;              // init all port B as outputs
    LATB = 0x000;               // init all port B as lo

    TMR32_init();
//    motor_init();
//    SM_init();
}

