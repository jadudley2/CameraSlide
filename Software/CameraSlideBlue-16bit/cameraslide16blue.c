/* 
 * File:   cameraslide16.c
 * Author: James Dudley
 *
 * Created on August 12, 2015, 11:47 PM
 */

#include "cameraslide16blue.h"

extern unsigned char UART1_rxFlag;

int main() {

    main_init();

    // toggle HB LED
    timeObj tStart = TMR32_getTime();
    timeObj tNow = tStart;
    
    while (1) {
        while(TMR32_timeDiff(tNow,tStart,MSEC) < 1000) {
            tNow = TMR32_getTime();
//            UART1_update();
        }
//        if (ADC_rawValue() >= 1000) {
//            SYS_LED_PIN = ~SYS_LED_PIN;
//        }

        unsigned char line[] = {'1','2','3','4','5','6','7','8','9','0'};
        UART1_writeLine(&line[0],sizeof(line));
//        if (UART1_update() != 0) {
//            unsigned char* ln = UART1_readLine(5);
//            if (ln[4] == 'k') {
//                SYS_LED_PIN = ~SYS_LED_PIN;
//            }
//        }
        
//        UART1_update();
//        moveSingleStep(HOME);
//        SYS_LED_PIN = ~SYS_LED_PIN;
//        PWR_LED_PIN = ~PWR_LED_PIN;
//        HB_LED_PIN = ~HB_LED_PIN;
//        SHUTTER_PIN = ~SHUTTER_PIN;
//        BT_RESET_PIN = ~BT_RESET_PIN;
        tStart = tNow;

//        unsigned char num = UART1_update();
//        if(num >= 4) {
//            unsigned char* chr2 = &(UART1_readLine(num)[0]);
//            if (chr2[0] == MSG_START_BYTE) {
//                command temp = {0,0};
//                temp.command = chr2[1];
//                temp.value = chr2[2];
//                UART1_writeChar(temp.command);
//                UART1_writeChar(temp.value);
//            }
//        }
//        if (checkCommandBuffer() != 0) {
//            command cmd = readCommand(0);
//            unsigned char* chr2 = &(cmd.command);
//            UART1_writeLine(chr2,1);
//        }

        //SM_mainStateMachine();
    }

    return (EXIT_SUCCESS);
}

void main_init(void) {

    AD1PCFGL = 0xFFF;           // init all ports as digital

    PMD1bits.I2C1MD = 1;        // disable I2C1 peripheral
    PMD1bits.PWM1MD = 1;        // disable PWM peripheral

    CM1CONbits.CON = 0;         // disable comp1 to use as digital I/O
    CM3CONbits.CON = 0;         // disable comp3 to use as digital I/O
    I2C1CONbits.I2CEN = 0;      // disable I2C1 to use as digital I/O
    IEC4bits.FLTB1IE = 0;       // disable PWM1 fault B enable
    PWM1CON1bits.PEN1H = 0;     // disable PWM1
    PWM1CON1bits.PEN1L = 0;

    TRISA = 0x000;              // init all port A as outputs
    LATA = 0x000;               // init all port A as lo
    TRISB = 0x000;              // init all port B as outputs
    LATB = 0x000;               // init all port B as lo

    // set AN0 as analog input (VCC_PWR_PIN)
    AD1PCFGL &= 0xFFE;      // analog
    TRISAbits.TRISA0 = 1;    // input

    // pin switches as digital inputs
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    __builtin_write_OSCCONL(OSCCON & 0xBF);
    OSCCONbits.IOLOCK = 0;
    // map U1TX to rp7, output
    RPOR3bits.RP7R = 3;
    // map U1RX to rp6, input
    TRISBbits.TRISB6 = INPUT;
    RPINR18bits.U1RXR = 6;
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    OSCCONbits.IOLOCK = 1;  // disable writes to peripheral mapping

    TMR32_init();
    UART1_init();
    BT_RESET_PIN = 1;
    motor_init(MOTOR_SPEED, MOTOR_STEPS_PER_REV);
    SM_init();

    // TODO: set A0 as analog input and monitor it for a voltage in Vmotor range
}

