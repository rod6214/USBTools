// #include <xc.h>
// #include <string.h>
// #include <pic18f2550.h>
#include "usbtypes.h"

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

const DeviceDescriptor_t 
device_descriptor = { //
    0x12, // bLength
	0x01, // bDescriptorType
    0x200, // bcdUSB lsb, bcdUSB msb
    0x00, // bDeviceClass
	0x00, // bDeviceSubClass
    0x00, // bDeviceProtocl
	USB_BUFFER_CONTROL_SIZE, // bMaxPacketSize
    0x048D, // idVendor lsb, idVendor msb
    0x003F, // idProduct lsb, idProduct msb
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
       0x03, // bInterfaceClass (3 = HID)
       0x00, // bInterfaceSubClass
       0x00, // bInterfaceProtocol (1) Keyboard, (2) Mouse
       0x00, // iInterface
  },
   {/*HID interface descriptor*/
       sizeof(HIDInterfaceDescriptor_t), // Length
       0x21, // bDescriptorType
       0x0111, // bcdHID
       0x00, // bCountryCode
       0x01, // bNumDescriptors
	   /*HID class interface descriptor*/
       0x22, // bDescriptorType
       HID_RPT01_SIZE,// wItemLength (HID report size)
   },
   {/*Enpoint 1 IN descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x81, // bEndpointAddress
       0x03, // bmAttributes
       USB_BUFFER_CONTROL_SIZE, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
   {/*Enpoint 1 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       0x03, // bmAttributes
       USB_BUFFER_CONTROL_SIZE, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
};

// Class specific descriptor - HID 
const struct {BYTE report[HID_RPT01_SIZE];}hid_rpt01={
{
    0x06, 0x00, 0xFF,       // Usage Page = 0xFF00 (Vendor Defined Page 1)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)
    0x19, 0x01,             //      Usage Minimum 
    0x29, 0x02,             //      Usage Maximum 	//64 input usages total (0x01 to 0x40)
    0x15, 0x01,             //      Logical Minimum (data bytes in the report may have minimum value = 0x00)
    0x25, 0x02,      	  	//      Logical Maximum (data bytes in the report may have maximum value = 0x00FF = unsigned 255)
    0x75, 0x08,             //      Report Size: 8-bit field size
    0x95, 0x02,             //      Report Count: Make sixty-four 8-bit fields (the next time the parser hits an "Input", "Output", or "Feature" item)
    0x81, 0x00,             //      Input (Data, Array, Abs): Instantiates input packet fields based on the above report size, count, logical min/max, and usage.
    0x19, 0x01,             //      Usage Minimum 
    0x29, 0x02,             //      Usage Maximum 	//64 output usages total (0x01 to 0x40)
    0x91, 0x00,             //      Output (Data, Array, Abs): Instantiates output packet fields.  Uses same report size and count as "Input" fields, since nothing new/different was specified to the parser since the "Input" item.
    0xC0}                   // End Collection
}; 
