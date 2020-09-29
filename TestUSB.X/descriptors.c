/*
 * File:   descriptors.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 7:11 PM
 */
#include "global.h"

const BYTE stringDescriptor[] = {
    4, 0x03, 0x09, 0x04
};

const BYTE iProductDesc[] = {
    8, 0x03, 'H', 'e', 'l', 'l', 'o', '\n'
};

const BYTE iSerialNumberDesc[] = {
    6, 0x03, '1', '2', '3', '4'
};

const BYTE* StringDescTable[] = {
    stringDescriptor,
    iProductDesc,
    iSerialNumberDesc
};

const unsigned char deviceDescriptor[] = {
    0x12, // bLength
    0x01, // bDescriptorType
    0x00,0x02, // bcdUSB
    0x00, // bDeviceClass
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    0x40, // bMaxPacketSize0
    0x08,0x04, // idVendor
    0x0A,0x00, // idProduct 
    0x01,0x00, // bcdDevice
    0x01, // iManufacturer
    0x02, // iProduct
    0x00, // iSerialnumber
    0x01 // bNumConfigurations
};

const unsigned char configurationDescriptior[] = {
    0x9, // Length
    0x02, // bDescriptorType
    0x29, 0x00, // Total length
    0x01, // NumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0xA0, // bmAttributes
    0xA0 // MaxPower
};

const unsigned char interfaceDescriptor[] = {
    0x9, // Length
    0x04, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints
    0x03, // bInterfaceClass (3 = HID)
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00 // iInterface
};

const unsigned char hidDescriptor[] = {
    0x9, // Length
    0x21, // bDescriptorType
    0x10,01, // bcdHID
    0x00, // bCountryCode
    0x01, // bNumDescriptors 
    0x22, // bDescriptorType
    0x2f,0x00// wItemLength
};

const unsigned char endpointINDescriptor[] = {
    0x7, // Length
    0x05, // bDescriptorType
    0x81, // bEndpointAddress
    0x03, // bmAttributes
    0x40, // MaxPacketSize (LITLE ENDIAN)
    0x0A, // bInterval
};

const unsigned char endpointOUTDescriptor[] = {
    0x7, // Length
    0x05, // bDescriptorType
    0x01, // bEndpointAddress
    0x03, // bmAttributes
    0x40, // MaxPacketSize (LITLE ENDIAN)
    0x0A, // bInterval
};
