/* 
 * File:   cameraslide16blue.h
 * Author: James Dudley
 *
 * Created on August 12, 2015, 10:14 PM
 */

#ifndef CAMERASLIDE16BLUE_H
#define	CAMERASLIDE16BLUE_H

#include "constants.h"
#include "TMR32.h"
#include "statemachines.h"
#include "motor.h"
#include "UART1.h"
#include "ADC.h"

int main(void);

void main_init(void);

// PIC24FJ32MC102 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

_CONFIG1(WDTPOST_PS32768
        & WDTPRE_PR128
        & PLLKEN_ON
        & WINDIS_OFF
        & FWDTEN_OFF
        & ICS_PGD1
        & HPOL_ON
        & GWRP_OFF
        & GCP_OFF
    );

_CONFIG2(POSCMOD_NONE
        & ALTI2C_OFF
        & LPOL_OFF
        & IOL1WAY_OFF
        & OSCIOFNC_ON
        & FCKSM_CSDCMD
        & FNOSC_FRCDIVN
        & WDTWIN_WDTWIN75
        & PWMPIN_ON
        & PWMLOCK_OFF
        & IESO_OFF
    );

#endif	/* CAMERASLIDE16BLUE_H */

