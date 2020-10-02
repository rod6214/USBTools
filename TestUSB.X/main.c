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
    TRISB = 0;
    // TRISB0 = 0;
    // TRISB1 = 0;
    // TRISB2 = 0;
//    PORTB = 7;
    
//    PORTBbits.RB0 = 1;
//    PORTBbits.RB0 = 0;
//    PORTBbits.RB1 = 0;
//    TRNIF = 0;
    di();
//    INTCONbits.INT0IE = 1;
//    USBIE = 1;
//    TRNIE = 1;
//    SOFIE =  1;
//    STALLIE = 1;
//    ACTVIE = 1;
//    URSTIE = 1;
//    USBIP = 1;
//    UIEbits.
    IPR2bits.USBIP = 1;
    INTCONbits.PEIE = 0;
    PIE2bits.USBIE = 1;
    // Reset USB interrupts    
    UIR = 0;
//    UIEbits.URSTIE = 0;// occur 2 times
    UIEbits.SOFIE = 0;// many times we couldn't count it
    UIEbits.TRNIE = 1;// occur 1 time
    UIEbits.ACTVIE = 1;// occur 1 time
    UIEbits.IDLEIE = 1;// occur 2 times
    UIEbits.URSTIE = 1;// occur 4 times
    STALLIE = 0; 
    UERRIE = 0;
    RCONbits.IPEN = 1;
    SUSPND = 0;
//    ei();
//    PIR2bits.USBIF = 1;
    INTCONbits.GIEH = 1;
//    INTCONbits.GIEL = 1;
//    TRNIF = 1;
//    di();
    
    
    externTest();
    while(1);
    
    return;
}
