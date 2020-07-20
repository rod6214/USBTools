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

volatile BDnSTAT_t BD0STAT __at(BDMEM(0));

void USB_enumerate() {
    
    switch(USTATbits.ENDP) {
        case EP0:
        {
            if (USTATbits.DIR) {
                // Process IN token (to host)
            }
            else {
                // Process OUT or SETUP token (from host)
                if (UADDR) {
                    // Device has been enumerated
                    
                }
                else {
                    // Could be SETUP
                    
                    
                    while(1) {
                        if (!BD0STAT.UOWN) {
                            // check PID
                            switch(BD0STAT.PID) {
                                case SETUP:
                                    break;
                                case IN:
                                    break;
                                case OUT:
                                    break;
                                case DATA0:
                                    break;
                                case DATA1:
                                    break;
                            }
                        }
                    }
                }
            }
        }
            break;
        case EP1:
        {
        }
            break;
        default:
            break;
    }
}

void __interrupt(high_priority) genInt(void) {
//    PORTB = deviceDescriptor[0];
    if (INT0F) {
        INT0F = 0;
    }
    else if (INT1IF) {
        INT1IF = 0;
    }
    else if (USBIF) {
        if (SOFIF) {}
        else if (STALLIF) {}
        else if (IDLEIF) {}
        else if (TRNIF) {}
        else if (ACTVIF) {}
        else if (UERRIF) {}
        else if (URSTIF) {}
    }
    
}
