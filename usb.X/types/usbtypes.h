/* 
 * File:   usbtypes.h
 * Author: NelsonAmador
 *
 * Created on October 7, 2020, 4:31 PM
 */

#ifndef USBTYPES_H
#define	USBTYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "usb.h"

typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef const unsigned char* codePtr;
typedef unsigned char* dataPtr;

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

// Every device request starts with an 8 unsigned char setup packet (USB 2.0, chap 9.3)
// with a standard layout.  The meaning of wValue and wIndex will
// vary depending on the request type and specific request.
typedef struct _setup_packet_struct
{
    BYTE bmrequesttype; // D7: Direction, D6..5: Type, D4..0: Recipient
    BYTE brequest;      // Specific request
    BYTE wvalue0;       // LSB of wValue
    BYTE wvalue1;       // MSB of wValue
    BYTE windex0;       // LSB of wIndex
    BYTE windex1;       // MSB of wIndex
    USHORT wlength;       // Number of unsigned chars to transfer if there's a data stage
    BYTE extra[56];     // Fill out to same size as Endpoint 0 max buffer
} setup_packet_struct;

typedef struct _BDT
{
    BYTE STAT;
    BYTE CNT;
    UINT ADDR;
} BDT; //Buffer Descriptor Table

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

#ifdef	__cplusplus
}
#endif

#endif	/* USBTYPES_H */