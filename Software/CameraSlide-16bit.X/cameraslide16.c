/* 
 * File:   cameraslide16.c
 * Author: James Dudley
 *
 * Created on May 23, 2015, 11:11 AM
 */

#include "cameraslide16.h"

int main() {

    main_init();

    while(ON) {
        SM_mainStateMachine();
    }

    return (EXIT_SUCCESS);
}

void main_init(void) {

    U1MODEbits.UARTEN = 0;      // disable uart1 to use as digital I/O
    CM2CONbits.CON = 0;         // disable comp2 to use as digital I/O
    SPI2STATbits.SPIEN = 0;     // disable spi2 to use as digital I/O
    SPI2CON1bits.DISSDO = 1;    // disable sdo2 to use as digital I/O
    SPI2CON1bits.DISSCK = 1;    // disable sck2 to use as digital I/O
    I2C1CONbits.I2CEN = 0;      // disable I2C to use as digital I/O
    
    ANSA = 0x000;               // init all port A as digital
    TRISA = 0x000;              // init all port A as outputs
    LATA = 0x000;               // init all port A as lo
    ANSB = 0x000;               // init all port B as digital
    TRISB = 0x000;              // init all port B as outputs
    LATB = 0x000;               // init all port B as lo

    TMR32_init();
    motor_init();
    SM_init();
}

