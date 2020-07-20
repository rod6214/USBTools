/*
 * File:   usb.c
 * Author: NelsonAmador
 *
 * Created on July 19, 2020, 11:56 AM
 */

//#include <xc.h>
//#include <pic18f2550.h>
//#include <string.h>
//#include "global.h"

//#define EP0 0
//#define EP1 1
//#define EP2 2
//#define EP3 3
//#define EP4 4
//
//#define OUT 1
//#define IN 9
//#define SOF 5
//#define SETUP 13
//#define DATA0 3
//#define DATA1 11
//#define DATA2 7
//#define MDATA 15
//#define ACK 2
//#define NAK 10
//#define STALL 14
//#define NYET 6
//
//#define GET_STATUS 0
//#define CLEAR_FEATURE 1
//#define SET_FEATURE 3
//#define SET_ADDRESS 5
//#define GET_DESCRIPTOR 6
//#define SET_DESCRIPTOR 7
//#define GET_CONFIGURATION 8
//#define SET_CONFIGURATION 9
//#define GET_INTERFACE 10
//#define SET_INTERFACE 11
//#define SYNCH_FRAME 12
//
//#define DEVICE 1
//#define CONFIGURATION 2
//#define STRING 3
//#define INTERFACE 4
//#define ENDPOINT 5
//#define DEVICE_QUALIFIER 6
//#define OTHER_SPEED_CONFIGURATION 7
//#define INTERFACE_POWER 8


//typedef unsigned char BYTE;
//
//const BYTE stringDescriptor[] = {
//    4, 0x03, 0x09, 0x04
//};
//
//const BYTE iProductDesc[] = {
//    8, 0x03, 'H', 'e', 'l', 'l', 'o', '\n'
//};
//
//const BYTE iSerialNumberDesc[] = {
//    6, 0x03, '1', '2', '3', '4'
//};
//
//const BYTE* StringDescTable[] = {
//    stringDescriptor,
//    iProductDesc,
//    iSerialNumberDesc
//};
//
//const unsigned char deviceDescriptor[] = {
//    0x12, // bLength
//    0x01, // bDescriptorType
//    0x00,0x02, // bcdUSB
//    0x00, // bDeviceClass
//    0x00, // bDeviceSubClass
//    0x00, // bDeviceProtocol
//    0x40, // bMaxPacketSize0
//    0xd8,0x04, // idVendor
//    0x01,0x00, // idProduct 
//    0x00,0x00, // bcdDevice
//    0x01, // iManufacturer
//    0x02, // iProduct
//    0x00, // iSerialnumber
//    0x01 // bNumConfigurations
//};
//
//const unsigned char configurationDescriptior[] = {
//    0x9, // Length
//    0x02, // bDescriptorType
//    0x29, 0x00, // Total length
//    0x01, // NumInterfaces
//    0x01, // bConfigurationValue
//    0x00, // iConfiguration
//    0xA0, // bmAttributes
//    0xA0 // MaxPower
//};
//
//const unsigned char interfaceDescriptor[] = {
//    0x9, // Length
//    0x04, // bDescriptorType
//    0x00, // bInterfaceNumber
//    0x00, // bAlternateSetting
//    0x02, // bNumEndpoints
//    0x03, // bInterfaceClass (3 = HID)
//    0x00, // bInterfaceSubClass
//    0x00, // bInterfaceProtocol
//    0x00 // iInterface
//};
//
//const unsigned char hidDescriptor[] = {
//    0x9, // Length
//    0x21, // bDescriptorType
//    0x10,01, // bcdHID
//    0x00, // bCountryCode
//    0x01, // bNumDescriptors 
//    0x22, // bDescriptorType
//    0x2f,0x00// wItemLength
//};
//
//const unsigned char endpointINDescriptor[] = {
//    0x7, // Length
//    0x05, // bDescriptorType
//    0x81, // bEndpointAddress
//    0x03, // bmAttributes
//    0x40, // MaxPacketSize (LITLE ENDIAN)
//    0x0A, // bInterval
//};
//
//const unsigned char endpointOUTDescriptor[] = {
//    0x7, // Length
//    0x05, // bDescriptorType
//    0x01, // bEndpointAddress
//    0x03, // bmAttributes
//    0x40, // MaxPacketSize (LITLE ENDIAN)
//    0x0A, // bInterval
//};

//extern void USB_init() {
//    // Set hardware configuration and attach
//    unsigned char* ucfg = ((unsigned char*)&UCFGbits);
//    unsigned char* uep0 = ((unsigned char*)&UEP0bits);
//    unsigned char* uep1 = ((unsigned char*)&UEP1bits);
//    
//    *ucfg = 0x54;
//    *uep0 = 0x17;
//    *uep1 = 0x17;
//}

//void load_deviceDescriptor(void* pDevdesc) {
//    BYTE len = deviceDescriptor[0];
//    if (pDevdesc) {
//        memcpy(pDevdesc, deviceDescriptor, len);
//    }
//}

//void load_configurationDescriptior(void* pDevdesc) {
//    BYTE len = deviceDescriptor[0];
//    if (pDevdesc) {
//        memcpy(pDevdesc, deviceDescriptor, len);
//    }
//}
//
//void load_interfaceDescriptor(void* pDevdesc) {
//    BYTE len = deviceDescriptor[0];
//    if (pDevdesc) {
//        memcpy(pDevdesc, deviceDescriptor, len);
//    }
//}
//
//void load_hidDescriptor(void* pHidDescriptor) {}
//
//void load_endpointINDescriptor(void* pEpDesc){}
//
//void load_endpointOUTDescriptor(void* pEpDesc){}
//
//void USB_enumerate() {
//    
//    switch(USTATbits.ENDP) {
//        case EP0:
//        {
//            if (USTATbits.DIR) {
//                // Process IN token (to host)
//            }
//            else {
//                // Process OUT or SETUP token (from host)
//                if (UADDR) {
//                    // Device has been enumerated
//                    
//                }
//                else {
//                    // Could be SETUP
//                    BD0STAT.adrh = 5;
//                    BD0STAT.adrl = 0;
//                    BD0STAT.DTS = 0;
//                    BD0STAT.KEN = 0;
//                    BD0STAT.DTSEN = 1;
//                    BD0STAT.BSTALL = 1;
//                    
//                    while(1) {
//                        if (!BD0STAT.UOWN) {
//                            // check PID
//                            switch(BD0STAT.PID) {
//                                case SETUP:
//                                    break;
//                                case IN:
//                                    break;
//                                case OUT:
//                                    break;
//                                case DATA0:
//                                    break;
//                                case DATA1:
//                                    break;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//            break;
//        case EP1:
//        {
//        }
//            break;
//        default:
//            break;
//    }
//}
//
//void __interrupt(high_priority) genInt(void) {
//    PORTB = deviceDescriptor[0];
//    if (INT0F) {
//        INT0F = 0;
//    }
//    else if (INT1IF) {
//        INT1IF = 0;
//    }
//    else if (USBIF) {
//        if (SOFIF) {}
//        else if (STALLIF) {}
//        else if (IDLEIF) {}
//        else if (TRNIF) {}
//        else if (ACTVIF) {}
//        else if (UERRIF) {}
//        else if (URSTIF) {}
//    }
//    
//}

//void interrupt_int0() {
//    PORTA = 1;
//}