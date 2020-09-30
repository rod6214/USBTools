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
    
#define EP0 0
#define EP1 1
#define EP2 2
#define EP3 3
#define EP4 4

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
    
// Buffer descriptor registers
// Buffer status bits
// #define _UOWN 7
// //--- CPU mode only ---/
// #define _DTS 6
// #define _KEN 5
// #define _INCDIS 4
// #define _DTSEN 3
// #define _BSTALL 2
// //---------------------/
// #define _BC9 1
// #define _BC8 0

// Buffer Descriptor bit masks (from PIC datasheet)
#define __UOWN   0x80 // USB Own Bit
#define __DTS    0x40 // Data Toggle Synchronization Bit
#define __KEN    0x20 // BD Keep Enable Bit
#define __INCDIS 0x10 // Address Increment Disable Bit
#define __DTSEN  0x08 // Data Toggle Synchronization Enable Bit
#define __BSTALL 0x04 // Buffer Stall Enable Bit
#define __BC9    0x02 // Byte count bit 9
#define __BC8    0x01 // Byte count bit 8

// USTAT register
#define DIR_IN 4
#define DIR_OUT 0

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

/* Class-Specific Requests */
#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23


typedef unsigned char BYTE;

#define E0SZ 16

typedef struct _BDT
{
    unsigned char STAT;
    unsigned char CNT;
    unsigned int ADDR;
} BDT; //Buffer Descriptor Table

//endpoints
volatile BDT ep0_o __at (0x0400+0*8);
volatile BDT ep0_i __at (0x0404+0*8);
volatile BDT ep1_o __at (0x0400+1*8);
volatile BDT ep1_i __at (0x0404+1*8);
volatile BDT ep2_o __at (0x0400+2*8);
volatile BDT ep2_i __at (0x0404+2*8);
volatile BDT ep3_o __at (0x0400+3*8);
volatile BDT ep3_i __at (0x0404+3*8);

// Descriptor Types
#define DEVICE_DESCRIPTOR        0x01
#define CONFIGURATION_DESCRIPTOR 0x02
#define STRING_DESCRIPTOR        0x03
#define INTERFACE_DESCRIPTOR     0x04
#define ENDPOINT_DESCRIPTOR      0x05
#define QUALIFIER_DESCRIPTOR     0x06

#define LOWBYTE(x) (x & 255)
#define HIGHBYTE(x) ((x >> 8) & 255) 

typedef struct _setup_packet_struct
{
    unsigned char bmrequesttype; // D7: Direction, D6..5: Type, D4..0: Recipient
    unsigned char brequest;      // Specific request
    unsigned char wvalue0;       // LSB of wValue
    unsigned char wvalue1;       // MSB of wValue
    unsigned char windex0;       // LSB of wIndex
    unsigned char windex1;       // MSB of wIndex
    unsigned short wlength;       // Number of unsigned chars to transfer if there's a data stage
    unsigned char extra[56];     // Fill out to same size as Endpoint 0 max buffer
} setup_packet_struct;

typedef struct {
    union {
        struct {
            unsigned BC8    :1;
            unsigned BC9    :1;
            unsigned        :6;
        };
        struct {
            unsigned        :2;
            unsigned BSTALL :1;
            unsigned DTSEN  :1;
            unsigned INCDIS :1;
            unsigned KEN    :1;
            unsigned DTS    :1;
            unsigned UOWN   :1;
        };
        struct {
            unsigned       :2;
            unsigned PID0  :1;
            unsigned PID1  :1;
            unsigned PID2  :1;
            unsigned PID3  :1;
            unsigned       :2;
        };
        struct {
            unsigned       :2;
            unsigned PID   :4;
            unsigned       :2;
        };
        unsigned char status;
    };
    unsigned char count;
    union {
        unsigned int address;
        struct {
            unsigned char adrl;
            unsigned char adrh;
        };
    };
} BDnSTAT_t;

typedef struct {
    unsigned char bLength;
    unsigned char bDescriptorType;
    uint16_t bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialnumber;
    unsigned char bNumConfigurations;
} DeviceDescriptor_t;

typedef struct {
    unsigned char bmRequestType;// 1
    unsigned char bRequest;// 1
    unsigned int wValue;// 2
    unsigned int wIndex;// 2
    unsigned int wLength;// 2
    void* data;
} USBRequest;

typedef struct {
    BDnSTAT_t* sts;
    USBRequest* request;
    DeviceDescriptor_t* descriptor;
} USB;

#define BDMEM(x) (400 + 4*x)

extern void load_descriptor(void* dest, void* srcDesc);
extern volatile BDnSTAT_t BD0STAT;
//extern volatile DeviceDescriptor_t BUFFER;
extern void externTest();

#ifdef	__cplusplus
}
#endif

#endif	/* USB_H */

