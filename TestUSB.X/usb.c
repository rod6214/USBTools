/*
 * File:   usb.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 8:37 PM
 */


#include <xc.h>
#include <string.h>
#include <pic18f2550.h>
#include "usb.h"

#define MAX_BUFFER_SIZE 0x2ff

static char table[1024] __at(0x400);
volatile BDnSTAT_t BD0STAT __at(0x400);
volatile BDnSTAT_t BD1STAT __at(0x404);
volatile DeviceDescriptor_t BUFFER __at(0x500);

//int __memCounter = 0;
char __transaction = 0;

static void preferredConfig(BYTE package);

void USB_enumeration() {
    if (BD0STAT.UOWN) {
        PORTB = 1;
    }
}

int ff = 1;

void __interrupt(low_priority) genIntL(void) {
    PORTBbits.RB1 = 1;
}

void __interrupt(high_priority) genIntH(void) {
    PORTBbits.RB1 = 1;
}

void USB_init() {
    // Set hardware configuration and attach
//    BYTE* ucfg = ((BYTE*)&UCFGbits);
//    BYTE* uep0 = ((BYTE*)&UEP0bits);
//    BYTE* uep1 = ((BYTE*)&UEP1bits);
//    
//    *ucfg = 0x54;
//    *uep0 = 0x17;
//    *uep1 = 0x17;
    
    UCFGbits.FSEN = 1;
    UCFGbits.UPUEN = 1;
    UCFGbits.UTRDIS = 0;
    UCFGbits.UOEMON = 0;
    UCFGbits.UTEYE = 0;
    UCFGbits.PPB = 0;
    
    UEP0bits.EP0HSHK = 1;
    UEP0bits.EP0CONDIS = 0;
    UEP0bits.EP0OUTEN = 1;
    UEP0bits.EP0INEN = 1;
    UEP0bits.EPSTALL = 1;
    
    
    UCONbits.SUSPND = 0;
    UCONbits.RESUME = 1;
//    BUFFER.bLength = 0;
    
//    while(1);
//    UCONbits.PKTDIS = 0;
    BD0STAT.UOWN = 0;
    BD1STAT.UOWN = 0;
//    TRNIF = 0;
    UCONbits.PKTDIS = 1;
//    BD0STAT.UOWN = 0;
    UCONbits.SE0 = 0;
    UCONbits.USBEN = 1;
    BD0STAT.UOWN = 1;
//    UCONbits.USBEN = 0;
//    TRNIF = 1;
    
//    while(!UCONbits.SE0);
//    
//    TRNIF = 1;
    
    while(1){
    }
    
    
    while(1);
    
//    while(UCONbits.PKTDIS);
//    PORTBbits.RB0 = 1;
//    if (BD0STAT.UOWN) {
//        PORTBbits.RB1 = 1;
//    }
//    while(!BD0STAT.UOWN);
    
    // Configure buffer descriptor
    
//    preferedConfig();
}

void externTest() {
    table[1023] = 0;
    preferredConfig(0);
    USB_init();
}

static void preferredConfig(BYTE package) {

    BD0STAT.adrl = 0x40;
    BD0STAT.adrh = 0x04;
//    BD0STAT.address = (unsigned int)&BUFFER;
//    BD1STAT.address = (unsigned int)&BUFFER + 0x40;
//    BD0STAT.DTS = 0;// This item must alternate data transaction
    BD0STAT.count = 0x12;
    BD0STAT.DTS = 0;
    BD0STAT.KEN = 0;
    BD0STAT.INCDIS = 0;
    BD0STAT.DTSEN = 1;
    BD0STAT.BSTALL = 1;
    BD0STAT.BC8 = 0;
    BD0STAT.BC9 = 0;
    
    BD1STAT.adrl = 0x40;
    BD1STAT.adrh = 0x05;
    BD1STAT.count = 0x08;
    BD1STAT.DTS = 1;
    BD1STAT.KEN = 0;
    BD1STAT.INCDIS = 0;
    BD1STAT.DTSEN = 1;
    BD1STAT.BSTALL = 1;
    BD1STAT.BC8 = 0;
    BD1STAT.BC9 = 0;
    
}
