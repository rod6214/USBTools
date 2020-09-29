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
#include "global.h"

#define MAX_BUFFER_SIZE 0x2ff

static char table[1024] __at(0x400);
// Buffer descriptor endpoint 0 OUT
volatile BDnSTAT_t BD0STAT __at(0x400);
// Buffer descriptor endpoint 0 IN
volatile BDnSTAT_t BD1STAT __at(0x404);
volatile unsigned char BUFFER0[64] __at(0x500);
volatile unsigned char BUFFER1[64] __at(0x540);
//volatile char BUFFER __at(0x500);


//int __memCounter = 0;
char __transaction = 0;

static void preferredConfig(BYTE package);
void flags();

void USB_enumeration() {
    if (BD0STAT.UOWN) {
        PORTB = 1;
    }
}

int ff = 1;

void __interrupt(low_priority) genIntL(void) {
    // PORTBbits.RB1 = 1;
    return;
}

static int counter = 0;

#define EP_CTRL 6
#define HSHK_EN 16

void _test01 () {
    //    USBRequest* requestOut = (USBRequest*)(&BUFFER0[0]);
//    USBRequest* requestIn = (USBRequest*)(&BUFFER1[0]);
    if (USBIF) {
        
        if(STALLIF)
        {
            if(UEP0bits.EPSTALL)
            {
                UEP0bits.EPSTALL = 0;
            }
            UIRbits.STALLIF = 0;
        }
        
        if (ACTVIF) {
            SUSPND = 0;
            ACTVIF = 0;
        }
        
//        if (counter) {
//            while(BD1STAT.UOWN);
//            PORTB++; 
//        }
        
        if(UERRIF) {
//            PORTB++;
            UERRIF = 0;
        }
        
        if (SOFIF) {
            SOFIF = 0;
        }
        
//        if (SUSPND) return;
        
        if(URSTIF)
        {
            
            while(UIRbits.TRNIF)            // Flush any pending transactions
            {
                UIRbits.TRNIF = 0;
                
            }
            UIR   = 0;                      // Clears all USB interrupts
            UIE   = 0x6B;                   // Enable all interrupts except ACTVIE & IDLE
            UADDR = 0;                      // Reset to default address
            UEP0  = EP_CTRL | HSHK_EN;      // Init EP0 as a Ctrl EP
//            USBPrepareForNextSetupTrf();    // ���������� BDT ��� ��ɣ�� SETUP
            UCONbits.PKTDIS = 0;            // Make sure packet processing is enabled
        }
        
        if (TRNIF){
            int pid = DIR ? BD1STAT.PID : BD0STAT.PID;
            
            if (USTAT == DIR_OUT) 
            {
            }
            
            PORTB++;
            
            if (PORTB >= 4){
                PORTC = 0;
                PORTC = 0;
                PORTC = 0;
            }
            
            switch(pid){
                case IN:
                {
                    BD0STAT.UOWN = 0;
                    BD1STAT.UOWN = 0;
                    while(UEP0bits.EPSTALL) { UEP0bits.EPSTALL = 0; }
//                    BD1STAT.BSTALL = 0;
////                    BD0STAT.BSTALL = 0;
//                    BD1STAT.count = 8;
//                    BD1STAT.address = 0x548;
//                    BD1STAT.DTSEN = 1;
//                    BD1STAT.DTS = 0;
//                    BD1STAT.UOWN = 1;
//                    PKTDIS = 0;
//                    TRNIF = 0;
//                    
//                    while(BD1STAT.UOWN);
//                    PORTB++;
                    while(TRNIF) TRNIF = 0;
                    BD1STAT.BSTALL = 0;
                    BD0STAT.BSTALL = 0;
                    BD0STAT.count = 0;
                    BD0STAT.address = 0x540;
                    BD0STAT.DTSEN = 0;
                    BD0STAT.DTS = 0;
                    BD0STAT.UOWN = 1;
                    BD1STAT.UOWN = 1;
                    PKTDIS = 0;
                    TRNIF = 0;
                    PORTB = 5;
                    BD1STAT.UOWN = 1;
//                    TRNIF = 0;
//                    BD0STAT.BSTALL = 0;
//                    BD1STAT.BSTALL = 0;
//                    BD1STAT.DTSEN = 1;
//                    BD1STAT.DTS = 1;
//                    BD1STAT.count = 2;
//                    BD1STAT.address = &(BUFFER1[16]);
//                    BD1STAT.UOWN = 1;
//                    while(TRNIF) TRNIF = 0;
                }
                return;
                case OUT:
                {
                    PORTB = 7;
                    while(TRNIF) TRNIF = 0;
                }
                return;
                case SETUP:
                {
//                    while(1);
//                    di();
                    // Data stage
                    USBRequest* request = (USBRequest*)(&BUFFER0[0]);
                    TRNIF = 0;
                    load_descriptor(&(BUFFER1[0]), deviceDescriptor);
                    BD0STAT.BSTALL = 0;
                    BD1STAT.BSTALL = 0;
                    BD1STAT.DTSEN = 1;
                    BD1STAT.DTS = 1;
                    BD1STAT.count = 16;
                    BD1STAT.address = &(BUFFER1[0]);
                    
                    
                    
//                    BD1STAT.DTS = 0;
//                    BD1STAT.count = 2;
//                    BD1STAT.address = &(BUFFER1[16]);
//                    PKTDIS = 0;
//                    BD1STAT.UOWN = 1;
//                    while(BD1STAT.UOWN);
//                    PORTB = 3;
                    
//                    BD0STAT.BSTALL = 0;
//                    BD1STAT.BSTALL = 0;
//                    BD1STAT.DTSEN = 1;
//                    BD1STAT.DTS = 0;
//                    BD1STAT.count = 2;
//                    BD1STAT.address = &(BUFFER1[16]);
                    
                    PKTDIS = 0;
//                    TRNIF = 0;
//                    BD0STAT.UOWN = 1;
                    BD1STAT.UOWN = 1;
                    TRNIF = 0;
                    
//                    while(TRNIF) TRNIF = 0;
                }
                return;
            }
        }
        
        if (IDLEIF) {
            SUSPND = 1;
            IDLEIF = 0;
        }
        USBIF = 0;
    }
    return;
}

void _test02 () {
    if (USBIF) {
        
        if(STALLIF)
        {
            if(UEP0bits.EPSTALL)
            {
                UEP0bits.EPSTALL = 0;
            }
            UIRbits.STALLIF = 0;
        }

        if (ACTVIF) {
            SUSPND = 0;
            ACTVIF = 0;
        }

        if (SOFIF) {
            SOFIF = 0;
        }

        if(URSTIF)
        {
            while(UIRbits.TRNIF)            // Flush any pending transactions
            {
                UIRbits.TRNIF = 0;
                
            }
            UIR   = 0;                      // Clears all USB interrupts
            UIE   = 0x6B;                   // Enable all interrupts except ACTVIE & IDLE
            UADDR = 0;                      // Reset to default address
            UEP0  = EP_CTRL | HSHK_EN;      // Init EP0 as a Ctrl EP
            UCONbits.PKTDIS = 0;            // Make sure packet processing is enabled
        }

        if (TRNIF) {
            if ((USTAT & DIR_IN) == DIR_IN) {
                PORTB = 1;
            }
            else {
                PORTB = 2;
            }
        }

        if (IDLEIF) {
            SUSPND = 1;
            IDLEIF = 0;
        }
        USBIF = 0;
    }
}

void __interrupt(high_priority) genIntH(void) {
    _test02();
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
    UEP0bits.EPSTALL = 0;
    
    
    UCONbits.SUSPND = 0;
    UCONbits.RESUME = 0;
//    BUFFER.bLength = 0;
    
//    while(1);
//    UCONbits.PKTDIS = 0;
//    BD0STAT.UOWN = 0;
//    BD1STAT.UOWN = 0;
//    TRNIF = 0;
    UCONbits.PKTDIS = 0;
//    BD0STAT.UOWN = 0;
//    UCONbits.SE0 = 0;
//    UCONbits.USBEN = 1;
    
//    INTCON2bits.TMR0IP = 1;
    INTCONbits.TMR0IE = 0;
    INTCONbits.TMR0IF = 0;
    UIRbits.URSTIF = 0;
    UIRbits.IDLEIF = 0;
    UIRbits.SOFIF = 0;
    PIR2bits.USBIF = 0;
    PKTDIS = 0;
//    while(!UIRbits.URSTIF);
//    UIRbits.URSTIF = 0;
//    UCONbits.USBEN = 0;
    BD0STAT.UOWN = 1;
//    BD1STAT.UOWN = 1;
    UCONbits.USBEN = 1;
//    TRNIF = 0;
    
//    for(int j = 0; j < 4; j++){
//        flags();
//    }
    while(1);
}

void flags() {
    
        if(URSTIF)
        {
            
            while(UIRbits.TRNIF)            // Flush any pending transactions
            {
                UIRbits.TRNIF = 0;
                
            }

            UADDR = 0;                      // Reset to default address
            UEP0  = EP_CTRL | HSHK_EN;      // Init EP0 as a Ctrl EP
            UCONbits.PKTDIS = 0;            // Make sure packet processing is enabled
        }
        
        while(!TRNIF);
    
            if (TRNIF){
            int pid = DIR ? BD1STAT.PID : BD0STAT.PID;
            
            PORTB++;
            
            switch(pid){
                case IN:
                {
                    
                    while(UEP0bits.EPSTALL) { UEP0bits.EPSTALL = 0; }
                    
//                    BD1STAT.BSTALL = 0;
//                    BD0STAT.BSTALL = 0;
                    BD0STAT.count = 0;
                    BD0STAT.address = 0x540;
                    BD0STAT.DTSEN = 0;
                    BD0STAT.DTS = 0;
                    BD0STAT.UOWN = 1;
//                    BD1STAT.UOWN = 1;
                    PKTDIS = 0;
                    TRNIF = 0;
                }
                return;
                case OUT:
                {
                    PORTB = 7;
                    while(TRNIF) TRNIF = 0;
                }
                return;
                case SETUP:
                {
                    load_descriptor(&(BUFFER1[0]), deviceDescriptor);
//                    BD0STAT.BSTALL = 0;
//                    BD1STAT.BSTALL = 0;
                    BD1STAT.DTSEN = 1;
                    BD1STAT.DTS = 1;
                    BD1STAT.count = 18;
                    BD1STAT.address = &(BUFFER1[0]);
                    PKTDIS = 0;
                    BD1STAT.UOWN = 1;
                    TRNIF = 0;
                }
                return;
            }
        }
}

void externTest() {
    table[1023] = 0;
    preferredConfig(0);
    USB_init();
}

static void preferredConfig(BYTE package) {
    
    BD0STAT.address = 0x500;
    BD0STAT.count = 64;
    BD0STAT.DTS = 0;
    BD0STAT.KEN = 0;
    BD0STAT.INCDIS = 0;
    BD0STAT.DTSEN = 1;
    BD0STAT.BSTALL = 0;
    BD0STAT.BC8 = 0;
    BD0STAT.BC9 = 0;
    BD0STAT.UOWN = 0;
    
//    BD1STAT.adrl = 0x40;
//    BD1STAT.adrh = 0x05;
    BD1STAT.address = 0x500;
    BD1STAT.count = 0;
    BD1STAT.DTS = 1;
    BD1STAT.KEN = 0;
    BD1STAT.INCDIS = 0;
    BD1STAT.DTSEN = 1;
    BD1STAT.BSTALL = 0;
    BD1STAT.BC8 = 0;
    BD1STAT.BC9 = 0;
    BD1STAT.UOWN = 0;
    
}
