/* 
 * File:   cameraslide16.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:47 PM
 */

#include "cameraslide16blue.h"

int main() {

    main_init();

//    timeObj tStart = TMR32_getTime();
//    timeObj tNow = tStart;
    
    while (1) {
//        while(TMR32_timeDiff(tNow,tStart,MSEC) < 10) {
//            tNow = TMR32_getTime();
//        }
//
//        if (ADC_rawValue() <= 200) {
//            HB_LED_PIN = LO;
//        } else {
//            HB_LED_PIN = HI;
//        }
//
//        tStart = tNow;
        
        SM_mainStateMachine();
    }

    return (EXIT_SUCCESS);
}

void main_init(void) {

    AD1PCFGL = 0xFFF;           // init all ports as digital

    PMD1bits.I2C1MD = 1;        // disable I2C1 peripheral

    CM1CONbits.CON = 0;         // disable comp1 to use as digital I/O
    CM3CONbits.CON = 0;         // disable comp3 to use as digital I/O
    I2C1CONbits.I2CEN = 0;      // disable I2C1 to use as digital I/O
    PWM1CON1bits.PEN1H = 0;     // disable PWM1
    PWM1CON1bits.PEN1L = 0;

    TRISA = 0x000;              // init all port A as outputs
    LATA = 0x000;               // init all port A as lo
    TRISB = 0x000;              // init all port B as outputs
    LATB = 0x000;               // init all port B as lo

    // set AN0 as analog input (VCC_PWR_PIN)
    AD1PCFGL &= 0xFFE;      // AN0 as analog
    TRISAbits.TRISA0 = 1;   // input

    // pin switches as digital inputs
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    OSCCONbits.LOCK = 0;
    RPOR3bits.RP7R = 3;         // map U1TX to rp7, output
    TRISBbits.TRISB6 = INPUT;   // map U1RX to rp6, input
    RPINR18bits.U1RXR = 6;
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    OSCCONbits.LOCK = 1;  // disable writes to peripheral mapping

    TMR32_init();
    UART1_init();
    ADC_init();
    HB_LED_PIN = LO;
    BT_RESET_PIN = HI;
}

