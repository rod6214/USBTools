/*
 * File:   main.c
 * Author: Nelson Amador
 *
 * Created on July 18, 2020, 2:37 PM
 */
#define _XTAL_FREQ 48000000L
#include <xc.h>
#include <pic18f2550.h>
#include <string.h>
#include "global.h"

void main(void) {
    
    PORTB = 0;
    TRISB0 = 0;
    TRISB1 = 0;
    
//    PORTBbits.RB0 = 1;
//    PORTBbits.RB0 = 0;
//    PORTBbits.RB1 = 0;
//    TRNIF = 0;
    di();
//    INTCONbits.INT0IE = 1;
    USBIE = 1;
    TRNIE = 1;
    SOFIE =  1;
    STALLIE = 1;
    ACTVIE = 1;
    URSTIE = 1;
    USBIP = 1;
    
    
    
    RCONbits.IPEN = 1;
//    INTCONbits.GIEH = 1;
//    INTCONbits.GIEL = 0;
//    TRNIF = 0;
//    di();
    ei();
    
    externTest();
    while(1);
    
    return;
}
