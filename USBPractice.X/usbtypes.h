/* 
 * File:   usbtypes.h
 * Author: Nelson
 *
 * Created on October 27, 2022, 12:28 PM
 */

#ifndef USBTYPES_H
#define	USBTYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef const unsigned char codePtr;
typedef unsigned char dataPtr;

#define FALSE 0
#define TRUE 1

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

/* Endpoint Transfer Type */
#define ATTRIBUTE_MODE_CTRL       0x00            //Control Transfer
#define ATTRIBUTE_MODE_ISO        0x01            //Isochronous Transfer
#define ATTRIBUTE_MODE_BULK       0x02            //Bulk Transfer
#define ATTRIBUTE_MODE_INT        0x03            //Interrupt Transfer

#define DEVICE 1
#define CONFIGURATION 2
#define STRING 3
#define INTERFACE 4
#define ENDPOINT 5
#define DEVICE_QUALIFIER 6
#define OTHER_SPEED_CONFIGURATION 7
#define INTERFACE_POWER 8

#define DETACHED        0
#define ATTACHED        1
#define POWERED         2
#define DEFAULT         3
#define ADDRESS         4
#define CONFIGURED      5
#define DATA_IN_STAGE      6
#define DATA_OUT_STAGE      7

#define USB_EP_BUFFER_LEN 64
#define USB_BUFFER_CONTROL_SIZE 8

/* Configuration Attributes */
#define _DEFAULT    0x01<<7         //Default Value (Bit 7 is set)
#define _SELF       0x01<<6         //Self-powered (Supports if set)
#define _RWU        0x01<<5         //Remote Wakeup (Supports if set)

typedef struct 
{
    uint8_t BDSTAT;
    uint8_t BDCNT;
    uint16_t ADDR;
} BD_t;

typedef struct 
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} TokenPacket_t;

/******************************************************************************
 * USB Interface Device Descriptor
 *****************************************************************************/
typedef struct {
    BYTE bLength;
    BYTE bDescriptorType;
    UINT bcdUSB;
    BYTE bDeviceClass;
    BYTE bDeviceSubClass;
    BYTE bDeviceProtocol;
    BYTE bMaxPacketSize0;
    UINT idVendor;
    UINT idProduct;
    UINT bcdDevice;
    BYTE iManufacturer;
    BYTE iProduct;
    BYTE iSerialnumber;
    BYTE bNumConfigurations;
} DeviceDescriptor_t;

/******************************************************************************
 * USB Interface Descriptor Structure
 *****************************************************************************/
typedef struct {
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bInterfaceNumber;
    BYTE bAlternateSetting;
    BYTE bNumEndpoints;
    BYTE bInterfaceClass;
    BYTE bInterfaceSubClass;
    BYTE bInterfaceProtocol;
    BYTE iInterface;
} InterfaceDescriptor_t;

/******************************************************************************
 * USB HID Interface Descriptor Structure
 *****************************************************************************/
typedef struct {
    BYTE bLength;
    BYTE bDescriptorType;
    UINT bcdHID;
    BYTE bCountryCode;
    BYTE bNumDescriptors;
    BYTE bClassDescriptorType;
    UINT wItemLength;
} HIDInterfaceDescriptor_t;

/******************************************************************************
 * USB HID Class Interface Descriptor Structure
 *****************************************************************************/
typedef struct {
    BYTE bDescriptorType;
    UINT wItemLength;
} HIDClassInterfaceDescriptor_t;

/******************************************************************************
 * USB Endpoint Descriptor Structure
 *****************************************************************************/
typedef struct {
    BYTE bLength;
    BYTE bDescriptorType;
    BYTE bEndpointAddress;
    BYTE bmAttributes;
    UINT wMaxPacketSize;
    BYTE bInterval;
} EndpointDescriptor_t;

/******************************************************************************
 * USB Configuration Descriptor Structure
 *****************************************************************************/
typedef struct {
    BYTE bLength;
    BYTE bDescriptorType;
    UINT wTotalLength;
    BYTE bNumInterfaces;
    BYTE bConfigurationValue;
    BYTE iConfiguration;
    BYTE bmAttributes;
    BYTE bMaxPower;
} ConfigurationDescriptior_t;

/******************************************************************************
 * USB Descriptor base
 *****************************************************************************/
typedef struct {
    ConfigurationDescriptior_t configDesc;
    InterfaceDescriptor_t interfaceDesc;
} Desc_t;

typedef struct {
    dataPtr buffer_rx;
    dataPtr buffer_tx;
    BYTE usb_device_state;
    BYTE device_address;
    BYTE current_configuration; // 0 or 1
    BYTE control_stage; // Holds the current stage in a control transfer
    BYTE request_handled; // Set to 1 if request was understood and processed.
    codePtr pDescriptor; // Data to host from FLASH
    BYTE dlen; // Number of unsigned chars of data
    BYTE max_buffer_size;
    BYTE setup_packet;
} USB;

#define SETUP 0x0D
#define IN 0x09
#define OUT 0x01
#define UOWN_bit 7
#define UOWN (1 << UOWN_bit)
#define DTS_bit 6
#define DTS (1 << DTS_bit)
#define KEN_bit 5
#define KEN (1 << KEN_bit)
#define INCDIS_bit 4
#define INCDIS (1 << INCDIS_bit)
#define DTSEN_bit 3
#define DTSEN (1 << DTSEN_bit)
#define BSTALL_bit 2
#define BSTALL (1 << BSTALL_bit)
#define BC8_bit 1
#define BC8 (1 << BC8_bit)
#define BC9_bit 0
#define BC9 (1 << BC9_bit)
#define IS_LAST_OUT (DIR == 0)
#define IS_CONTROL (((USTAT >> 3)&0xf)==0)
#define IS_SETUP(x) (((x.BDSTAT >> 2)&0xf)==SETUP)
#define IS_IN_EP0 (USTAT == 4)
#define IS_OUT_EP0 (USTAT == 0)


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
#ifdef	__cplusplus
}
#endif

#endif	/* USBTYPES_H */

