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

typedef unsigned char BYTE;
typedef unsigned int UINT;
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

/******************************************************************************
 * USB Device and configuration descriptors
 *****************************************************************************/
typedef struct {
	ConfigurationDescriptior_t configDesc;
	// Interface 1
    InterfaceDescriptor_t interfaceDesc;
    HIDInterfaceDescriptor_t hidInterfaceDesc1;
    EndpointDescriptor_t ep1_i;
    EndpointDescriptor_t ep1_o;
	// TODO: Set more interfaces here
} config_struct;

/******************************************************************************
 * USB Descriptor base
 *****************************************************************************/
typedef struct {} Desc_t;

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