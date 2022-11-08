/* 
 * File:   global.h
 * Author: Nelson
 *
 * Created on October 25, 2022, 8:51 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "usbtypes.h"
    extern volatile BD_t BD0_out;
    extern volatile BD_t BD0_in;
    extern volatile BD_t BD1_out;
    extern volatile BD_t BD1_in;
    extern int usb_device_status;
    extern void USB_init(void);
    extern void usb_read(int ep_addr);
#define USB_WAIT() while(usb_device_status != CONFIGURED){ \
        __delay_ms(1); \
    }
#define FLUSH(ppe) while ((ppe.BDSTAT & UOWN)!=0); \
            if (ppe.BDSTAT & DTS)   \
                ppe.BDSTAT = UOWN | DTSEN;  \
            else    \
                ppe.BDSTAT = UOWN | DTSEN | DTS;  
#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

