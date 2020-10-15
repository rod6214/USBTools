// #include <xc.h>
// #include <string.h>
// #include <pic18f2550.h>
#include "usbtypes.h"
#include "descriptor.h"

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
       0xC0, // bmAttributes
       50, // MaxPower (200mA)
   },
//    Interface 1
   {/*Interface descriptor*/
       sizeof(InterfaceDescriptor_t), // Length
       0x04, // bDescriptorType
       0x00, // bInterfaceNumber
       0x00, // bAlternateSetting
       0x02, // bNumEndpoints
       0xFF, // bInterfaceClass (Defined by the vendor)
       0xFF, // bInterfaceSubClass (Defined by the vendor)
       0xFF, // bInterfaceProtocol (Defined by the vendor)
       0x00, // iInterface
  },
   {/*Enpoint 1 IN descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x81, // bEndpointAddress
       ATTRIBUTE_MODE_BULK, // bmAttributes
       USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
   {/*Enpoint 1 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       ATTRIBUTE_MODE_BULK, // bmAttributes
       USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
//    {/*Enpoint 2 IN descriptor*/
//        sizeof(EndpointDescriptor_t), // Length
//        0x05, // bDescriptorType
//        0x81, // bEndpointAddress
//        ATTRIBUTE_MODE_BULK, // bmAttributes
//        USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
//        1, // bInterval
//    },
   {/*Enpoint 2 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x02, // bEndpointAddress
       ATTRIBUTE_MODE_BULK, // bmAttributes
       USB_EP_BUFFER_LEN, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
};

// // Class specific descriptor - HID 
// const struct _hid_report {BYTE report[HID_RPT01_SIZE];}hid_rpt01={
// {
//     0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
//     0x09, 0x01,             // Usage (Vendor Usage 1)
//     0xA1, 0x01,             // Collection (Application)
//     0x19, 0x00,             //      Usage Minimum 
//     0x29, 0x40,             //      Usage Maximum 	//64 input usages total (0x01 to 0x40)
//     0x15, 0x00,             //      Logical Minimum (data bytes in the report may have minimum value = 0x00)
//     0x25, 0x40,      	  	//      Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
//     0x75, 0x08,             //      Report Size: 8-bit field size
//     0x95, 0x40,             //      Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
//     0x81, 0x00,             //      Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
//     0x19, 0x00,             //      Usage Minimum 
//     0x29, 0x40,             //      Usage Maximum 	//64 output usages total (0x01 to 0x40)
//     0x91, 0x00,             //      Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
//     0xC0}                   // End Collection
// };

const BYTE string_descriptor0[] = { // available languages  descriptor
    0x04, STRING_DESCRIPTOR, //
    0x09, 0x04, //
};

const BYTE string_descriptor1[] = { //
    54, STRING_DESCRIPTOR, // bLength, bDscType
	'M','\0',
	'i','\0', 
	'c','\0',
	'r','\0',
	'o','\0',
	'c','\0',
	'h','\0',
	'i','\0',
	'p','\0',
	' ','\0',
	'T','\0',
	'e','\0',
	'c','\0',
	'h','\0',
	'n','\0',
	'o','\0',
	'l','\0',
	'o','\0',
	'g','\0',
	'y','\0',
	' ','\0',
	'I','\0',
	'n','\0',
	'c','\0',
	'.','\0',
	'\0','\0',
};
const BYTE string_descriptor2[] = { //
    0x30, STRING_DESCRIPTOR, //
    'S', 0x00, //
    'i', 0x00, //
    'm', 0x00, //
    'p', 0x00, //
    'l', 0x00, //
    'e', 0x00, //
    ' ', 0x00, //
    'H', 0x00, //
    'I', 0x00, //
    'D', 0x00, //
    ' ', 0x00, //
    'D', 0x00, //
    'e', 0x00, //
    'v', 0x00, //
    'i', 0x00, //
    'c', 0x00, //
    'e', 0x00, //
    ' ', 0x00, //
    'D', 0x00, //
    'e', 0x00, //
    'm', 0x00, //
    'o', 0x00, //
    '\0','\0',
};

const BYTE *string_descriptors[] = {
    string_descriptor0,
    string_descriptor1,
    string_descriptor2
};
