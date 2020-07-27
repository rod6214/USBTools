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

volatile BDnSTAT_t BD0STAT __at(BDMEM(0));
volatile BDnSTAT_t BD1STAT __at(BDMEM(1));
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

void __interrupt(high_priority) genInt(void) {
    
    if (TRNIF && ff) {
        USB_enumeration();
        ff = 1;
    }
    
}

void USB_init() {
    // Set hardware configuration and attach
    BYTE* ucfg = ((BYTE*)&UCFGbits);
    BYTE* uep0 = ((BYTE*)&UEP0bits);
    BYTE* uep1 = ((BYTE*)&UEP1bits);
    
    *ucfg = 0x54;
    *uep0 = 0x17;
    *uep1 = 0x17;
    UCONbits.USBEN = 1;
    UCONbits.SUSPND = 0;
    UCONbits.RESUME = 0;
    
    // Configure buffer descriptor
    BD0STAT.address = (unsigned int)&BUFFER;
    BD1STAT.address = (unsigned int)&BUFFER + 0x40;
//    preferedConfig();
}

void externTest() {
    USB_init();
}

static void preferredConfig(BYTE package) {
//    BD0STAT.DTS = 0;// This item must alternate data transaction
    BD0STAT.DTS = package;
    BD0STAT.KEN = 0;
    BD0STAT.INCDIS = 0;
    BD0STAT.DTSEN = 1;
    BD0STAT.BSTALL = 1;
}
