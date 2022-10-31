#include "global.h"

#include "usbtypes.h"
#include "descriptors.h"

const DeviceDescriptor_t 
device_descriptor = { //
    0x12, // bLength
	0x01, // bDescriptorType
    0x200, // bcdUSB lsb, bcdUSB msb
    0x00, // bDeviceClass
	0x00, // bDeviceSubClass
    0x00, // bDeviceProtocl
	USB_BUFFER_CONTROL_SIZE, // bMaxPacketSize
    0x04D8, // idVendor lsb, idVendor msb
    0x0053, // idProduct lsb, idProduct msb
    0x0002, // bcdDevice lsb, bcdDevice msb
    0x01, // iManufacturer
	0x02, // iProduct
    0x00, // iSerialNumber (none) 
	0x01  // bNumConfigurations*/
};

const config_struct
config_descriptor = {
   {/*Configuration descriptor*/
       sizeof(ConfigurationDescriptior_t), // Length
       0x02, // bDescriptorType
       sizeof(config_struct), // Total length
       0x01, // NumInterfaces
       0x01, // bConfigurationValue
       0x00, // iConfiguration
       _DEFAULT | _RWU, // bmAttributes
       50, // MaxPower (200mA)
   },
//    Interface 1
   {/*Interface descriptor*/
       sizeof(InterfaceDescriptor_t), // Length
       0x04, // bDescriptorType
       0x00, // bInterfaceNumber
       0x00, // bAlternateSetting
       0x02, // bNumEndpoints
       0x00, // bInterfaceClass (Defined by the vendor)
       0x00, // bInterfaceSubClass (Defined by the vendor)
       0x00, // bInterfaceProtocol (Defined by the vendor)
       0x00, // iInterface
  },
   {/*Enpoint 1 IN descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x81, // bEndpointAddress
       ATTRIBUTE_MODE_BULK, // bmAttributes
       USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
       32, // bInterval
   },
   {/*Enpoint 1 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       ATTRIBUTE_MODE_BULK, // bmAttributes
       USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
       32, // bInterval
   },
};

const BYTE string_descriptor0[] = { // available languages  descriptor
    0x04, STRING_DESCRIPTOR, //
    0x09, 0x04, //
};

const BYTE string_descriptor1[] = { //
    54, STRING_DESCRIPTOR, // bLength, bDscType
	'S','\0',
	'e','\0', 
	'm','\0',
	'a','\0',
	'l','\0',
	'e','\0',
	'r','\0',
	' ','\0',
	'I','\0',
	'n','\0',
	'c','\0',
	'.','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	' ','\0',
	'\0','\0',
};
const BYTE string_descriptor2[] = { //
    0x30, STRING_DESCRIPTOR, //
    'P', 0x00, //
    'I', 0x00, //
    'C', 0x00, //
    ' ', 0x00, //
    'D', 0x00, //
    'e', 0x00, //
    'm', 0x00, //
    'o', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    ' ', 0x00, //
    '\0','\0',
};

const BYTE *string_descriptors[] = {
    string_descriptor0,
    string_descriptor1,
    string_descriptor2
};