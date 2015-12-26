/*
 * File:   ADC.c
 * Author: James Dudley
 *
 * Created on November 21, 2015, 6:31 PM
 */

#include "ADC.h"

unsigned char ADC_flag = 0;

void ADC_init(void) {
    PMD1bits.AD1MD = 0;     // enable ADC1 module
    AD1CON3bits.ADCS = 0b00001111;  // Td = 16*Tcy
    AD1CON2bits.CHPS = 0b00;    // select channel 0
    AD1CON1bits.SSRC = 0b111;   // internal counter starts conversion
    AD1CON3bits.SAMC = 0b00011; // 3*Td
    AD1CON1bits.FORM = 0b00;    // integer data output
    AD1CON1bits.ASAM = 1;       // auto-start sampling on conversion complete
    AD1CON2bits.BUFM = 0;       // always fills buffer from beginning
    AD1CHS0bits.CH0NA = 0;      // ch 0 negative is Vss
    AD1CHS0bits.CH0SA = 0;      // ch 0 positive is AN0
    AD1CON1bits.ADON = 1;   // turn ADC1 module ON
    IFS0bits.AD1IF = 0;     // clear ADC1 interrupt flag
    IPC3bits.AD1IP = 4;     // interrupt priority
    IEC0bits.AD1IE = 1;     // enable ADC1 interrupt
}

unsigned char ADC_newValue(void) {
    return ADC_flag;
}

int ADC_rawValue(void) {
    return ADC1BUF0;
}