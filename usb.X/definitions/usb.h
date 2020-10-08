/* 
 * File:   usb.h
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 6:53 PM
 */

#ifndef USB_H
#define	USB_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <pic18f2550.h>

/** D E F I N I T I O N S ****************************************************/
#define EP0 0
#define EP1 1
#define EP2 2
#define EP3 3
#define EP4 4
#define EP5 5
#define EP6 6
#define EP7 7
#define EP8 8
#define EP9 9
#define EP10 10
#define EP11 11
#define EP12 12
#define EP13 13
#define EP14 14
#define EP15 15

#define OUT 1
#define IN 9
#define SOF 5
#define SETUP 13
#define DATA0 3
#define DATA1 11
#define DATA2 7
#define MDATA 15
#define ACK 2
#define NAK 10
#define STALL 14
#define NYET 6

#define GET_STATUS 0
#define CLEAR_FEATURE 1
#define SET_FEATURE 3
#define SET_ADDRESS 5
#define GET_DESCRIPTOR 6
#define SET_DESCRIPTOR 7
#define GET_CONFIGURATION 8
#define SET_CONFIGURATION 9
#define GET_INTERFACE 10
#define SET_INTERFACE 11
#define SYNCH_FRAME 12

#define DEVICE 1
#define CONFIGURATION 2
#define STRING 3
#define INTERFACE 4
#define ENDPOINT 5
#define DEVICE_QUALIFIER 6
#define OTHER_SPEED_CONFIGURATION 7
#define INTERFACE_POWER 8

// Buffer Descriptor bit masks (from PIC datasheet)
#define UOWN   0x80 // USB Own Bit
#define DTS    0x40 // Data Toggle Synchronization Bit
#define KEN    0x20 // BD Keep Enable Bit
#define INCDIS 0x10 // Address Increment Disable Bit
#define DTSEN  0x08 // Data Toggle Synchronization Enable Bit
#define BSTALL 0x04 // Buffer Stall Enable Bit
#define BC9    0x02 // Byte count bit 9
#define BC8    0x01 // Byte count bit 8

//
// Standard Request Codes USB 2.0 Spec Ref Table 9-4
//
#define GET_STATUS         0
#define CLEAR_FEATURE      1
#define SET_FEATURE        3
#define SET_ADDRESS        5
#define GET_DESCRIPTOR     6
#define SET_DESCRIPTOR     7
#define GET_CONFIGURATION  8
#define SET_CONFIGURATION  9
#define GET_INTERFACE     10
#define SET_INTERFACE     11
#define SYNCH_FRAME       12

// Standard Feature Selectors
#define DEVICE_REMOTE_WAKEUP    0x01
#define ENDPOINT_HALT           0x00

// Descriptor Types
#define DEVICE_DESCRIPTOR        0x01
#define CONFIGURATION_DESCRIPTOR 0x02
#define STRING_DESCRIPTOR        0x03
#define INTERFACE_DESCRIPTOR     0x04
#define ENDPOINT_DESCRIPTOR      0x05
#define QUALIFIER_DESCRIPTOR     0x06
#define HID_DESCRIPTOR           0x21
#define	REPORT_DESCRIPTOR        0x22
#define	PHYSICAL_DESCRIPTOR      0x23

// Device states (Chap 9.1.1)
#define DETACHED     0
#define ATTACHED     1
#define POWERED      2
#define DEFAULT      3
#define ADDRESS      4
#define CONFIGURED   5

// See USB spec chapter 5
#define SETUP_STAGE    0
#define DATA_OUT_STAGE 1
#define DATA_IN_STAGE  2
#define STATUS_STAGE   3

/******************************************************************************
 * USB Endpoint Definitions
 * USB Standard EP Address Format: DIR:X:X:X:EP3:EP2:EP1:EP0
 * This is used in the descriptors. See autofiles\usbdsc.c
 * 
 * NOTE: Do not use these values for checking against USTAT.
 * To check against USTAT, use values defined in "system\usb\usbdrv\usbdrv.h"
 *****************************************************************************/
#define _EP01_OUT   0x01
#define _EP01_IN    0x81
#define _EP02_OUT   0x02
#define _EP02_IN    0x82
#define _EP03_OUT   0x03
#define _EP03_IN    0x83
#define _EP04_OUT   0x04
#define _EP04_IN    0x84
#define _EP05_OUT   0x05
#define _EP05_IN    0x85
#define _EP06_OUT   0x06
#define _EP06_IN    0x86
#define _EP07_OUT   0x07
#define _EP07_IN    0x87
#define _EP08_OUT   0x08
#define _EP08_IN    0x88
#define _EP09_OUT   0x09
#define _EP09_IN    0x89
#define _EP10_OUT   0x0A
#define _EP10_IN    0x8A
#define _EP11_OUT   0x0B
#define _EP11_IN    0x8B
#define _EP12_OUT   0x0C
#define _EP12_IN    0x8C
#define _EP13_OUT   0x0D
#define _EP13_IN    0x8D
#define _EP14_OUT   0x0E
#define _EP14_IN    0x8E
#define _EP15_OUT   0x0F
#define _EP15_IN    0x8F

/* Configuration Attributes */
#define _DEFAULT    0x01<<7         //Default Value (Bit 7 is set)
#define _SELF       0x01<<6         //Self-powered (Supports if set)
#define _RWU        0x01<<5         //Remote Wakeup (Supports if set)

/* Endpoint Transfer Type */
#define _CTRL       0x00            //Control Transfer
#define _ISO        0x01            //Isochronous Transfer
#define _BULK       0x02            //Bulk Transfer
#define _INT        0x03            //Interrupt Transfer

/* Isochronous Endpoint Synchronization Type */
#define _NS         0x00<<2         //No Synchronization
#define _AS         0x01<<2         //Asynchronous
#define _AD         0x02<<2         //Adaptive
#define _SY         0x03<<2         //Synchronous

/* Isochronous Endpoint Usage Type */
#define _DE         0x00<<4         //Data endpoint
#define _FE         0x01<<4         //Feedback endpoint
#define _IE         0x02<<4         //Implicit feedback Data endpoint

#define IS_SETUP(epx) (((epx.STAT & 0x3C) >> 2) == SETUP)

#define IS_IN_EPx(x) ((USTAT) & (x << 3) & (1 << 2)) == ((x << 3) & (1 << 2))
#define IS_OUT_EPx(x) ((USTAT) & (x << 3)) == (x << 3)

#define IS_IN_EP0 ((USTAT) & (1 << 2)) == (1 << 2)
#define IS_OUT_EP0 (USTAT == 0)
#define IS_IN_EP1 IS_IN_EPx(1)
#define IS_OUT_EP1 IS_OUT_EPx(1)
#define IS_IN_EP2 IS_IN_EPx(2)
#define IS_OUT_EP2 IS_OUT_EPx(2)
#define IS_IN_EP3 IS_IN_EPx(3)
#define IS_OUT_EP3 IS_OUT_EPx(3)
#define IS_IN_EP4 IS_IN_EPx(4)
#define IS_OUT_EP4 IS_OUT_EPx(4)
#define IS_IN_EP5 IS_IN_EPx(5)
#define IS_OUT_EP5 IS_OUT_EPx(5)
#define IS_IN_EP6 IS_IN_EPx(6)
#define IS_OUT_EP6 IS_OUT_EPx(6)
#define IS_IN_EP7 IS_IN_EPx(7)
#define IS_OUT_EP7 IS_OUT_EPx(7)
#define IS_IN_EP8 IS_IN_EPx(8)
#define IS_OUT_EP8 IS_OUT_EPx(8)
#define IS_IN_EP9 IS_IN_EPx(9)
#define IS_OUT_EP9 IS_OUT_EPx(9)
#define IS_IN_EP10 IS_IN_EPx(10)
#define IS_OUT_EP10 IS_OUT_EPx(10)
#define IS_IN_EP11 IS_IN_EPx(11)
#define IS_OUT_EP11 IS_OUT_EPx(11)
#define IS_IN_EP12 IS_IN_EPx(12)
#define IS_OUT_EP12 IS_OUT_EPx(12)
#define IS_IN_EP13 IS_IN_EPx(13)
#define IS_OUT_EP13 IS_OUT_EPx(13)
#define IS_IN_EP14 IS_IN_EPx(14)
#define IS_OUT_EP14 IS_OUT_EPx(14)
#define IS_IN_EP15 IS_IN_EPx(15)
#define IS_OUT_EP15 IS_OUT_EPx(15)


#ifdef	__cplusplus
}
#endif

#endif	/* USB_H */

