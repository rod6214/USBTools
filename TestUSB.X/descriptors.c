/*
 * File:   descriptors.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 7:11 PM
 */
#include "global.h"
#include "descriptors.h"

#ifdef SPLIT_CONFIGURATION_DESCRIPTOR
const unsigned char deviceDescriptor[] = {
    0x12, // bLength
    0x01, // bDescriptorType
    0x00,0x02, // bcdUSB
    0x00, // bDeviceClass
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    0x40, // bMaxPacketSize0
    0xD8,0x04, // idVendor
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
    100 // MaxPower (200mA)
};

const unsigned char interfaceDescriptor[] = {
    0x9, // Length
    0x04, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoints
    0x03, // bInterfaceClass (3 = HID)
    0x01, // bInterfaceSubClass
    0x02, // bInterfaceProtocol
    0x00 // iInterface
};

const unsigned char hidInterfaceDescriptor[] = {
    0x9, // Length
    0x21, // bDescriptorType
    0x01,01, // bcdHID
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
#else

#define TOTAL_CONFIGURATION_LENGTH (sizeof(ConfigurationDescriptior_t) + \
sizeof(InterfaceDescriptor_t) + sizeof(HIDInterfaceDescriptor_t) + \
sizeof(EndpointDescriptor_t) + sizeof(EndpointDescriptor_t))

const BYTE stringDescriptor[] = {
    4, 0x03, 0x09, 0x04
};

const BYTE iProductDesc[] = {
    12, 0x03, 
    'H', 0x00,
    'e', 0x00,
    'l', 0x00,
    'l', 0x00,
    'o', 0x00,
};

const BYTE iManufacturerDesc[] = {
    0x20, 0x03, //
    'U', 0x00, //
    'S', 0x00, //
    'B', 0x00, //
    ' ', 0x00, //
    'G', 0x00, //
    'e', 0x00, //
    'n', 0x00, //
    'e', 0x00, //
    'r', 0x00, //
    'i', 0x00, //
    'c', 0x00, //
    ' ', 0x00, //
    'C', 0x00, //
    'D', 0x00, //
    'C', 0x00, //
};
// const BYTE iManufacturerDesc[] = {
//     22, 0x03, 
//     'N', 0x00,
//     'E', 0x00,
//     'L', 0x00,
//     'S', 0x00,
//     'O', 0x00,
//     'N', 0x00,
//     ' ', 0x00,
//     'I', 0x00,
//     'N', 0x00,
//     'C', 0x00,
// };

// const BYTE iSerialNumberDesc[] = {
//     10, 0x03, 
//     '1', 0x00,
//     '2', 0x00,
//     '3', 0x00,
//     '4', 0x00,
// };

const BYTE* StringDescTable[] = {
    stringDescriptor,
    iManufacturerDesc,
    iProductDesc,
    // iSerialNumberDesc
};

const DeviceDescriptor_t deviceDescriptor = {
    0x12, // bLength
    0x01, // bDescriptorType
    0x200, // bcdUSB
    0x00, // bDeviceClass
    0x00, // bDeviceSubClass
    0x00, // bDeviceProtocol
    0x40, // bMaxPacketSize0
    0x04D8, // idVendor
    0x0D, // idProduct 
    0x01, // bcdDevice
    0x01, // iManufacturer
    0x02, // iProduct
    0x00, // iSerialnumber
    0x01 // bNumConfigurations
};
const struct HIDConfigurationDescriptior_t configurationDesc = {
   {/*Configuration descriptor*/
       0x09, // Length
       0x02, // bDescriptorType
       sizeof(configurationDesc), //    0x29, // Total length
       0x01, // NumInterfaces
       0x01, // bConfigurationValue
       0x00, // iConfiguration
       0xA0, // bmAttributes
       100 // MaxPower (200mA)
   },
   {/*Interface descriptor*/
       0x09, // Length
       0x04, // bDescriptorType
       0x00, // bInterfaceNumber
       0x00, // bAlternateSetting
       0x02, // bNumEndpoints
       0x03, // bInterfaceClass (3 = HID)
       0x01, // bInterfaceSubClass
       0x02, // bInterfaceProtocol
       0x00 // iInterface
   },
   {/*HID interface descriptor*/
       0x09, // Length
       0x21, // bDescriptorType
       0x101, // bcdHID
       0x00, // bCountryCode
       0x01, // bNumDescriptors
   },
   {/*HID class interface descriptor*/
       0x22, // bDescriptorType
       0x32// wItemLength (HID report size)
   },
   {/*Enpoint 1 IN descriptor*/
       0x07, // Length
       0x05, // bDescriptorType
       0x81, // bEndpointAddress
       0x03, // bmAttributes
       0x40, // MaxPacketSize (LITLE ENDIAN)
       0x0A, // bInterval
   },
   {/*Endpoint 1 OUT descriptor*/
       0x07, // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       0x03, // bmAttributes
       0x40, // MaxPacketSize (LITLE ENDIAN)
       0x0A, // bInterval
   },
};

#endif