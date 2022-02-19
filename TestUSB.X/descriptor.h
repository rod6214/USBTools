#ifndef USB_DESCRIPTOR_H
#define USB_DESCRIPTOR_H

// Device and configuration descriptors
typedef struct {
	ConfigurationDescriptior_t configDesc;
	// Interface 1
    InterfaceDescriptor_t interfaceDesc;
    HIDInterfaceDescriptor_t hidInterfaceDesc1;
    EndpointDescriptor_t ep1_i;
    EndpointDescriptor_t ep1_o;
	// TODO: Set more interfaces here
} config_struct;

const unsigned char device_descriptor[] = { //
    0x12, // bLength
	0x01, // bDescriptorType
    0x00, 0x02, // bcdUSB lsb, bcdUSB msb
    0x00, // bDeviceClass
	0x00, // bDeviceSubClass
    0x00, // bDeviceProtocl
	ENDPOINT_0_SIZE, // bMaxPacketSize
    0x8D, 0x04, // idVendor lsb, idVendor msb
    0x3F, 0x00, // idProduct lsb, idProduct msb
    0x02, 0x00, // bcdDevice lsb, bcdDevice msb
    0x01, // iManufacturer
	0x02, // iProduct
    0x00, // iSerialNumber (none) 
	0x01  // bNumConfigurations*/
};

const unsigned char device_qualifier_descriptor[] = { //
    0x0A, 0x06, // bLength, bDescriptorType
    0x00, 0x02, // bcdUSB lsb, bcdUSB msb
    0x02, 0x00, // bDeviceClass, bDeviceSubClass
    0x00, ENDPOINT_0_SIZE, // bDeviceProtocl, bMaxPacketSize
    0x01, 0x00 // iSerialNumber, bNumConfigurations*/
};

static const char const_values_0x00_0x00[] = { 0, 0 };
static const char const_values_0x00_0x01[] = { 0, 1 };
static const char const_values_0x01_0x00[] = { 1, 0 };
static const char const_values_status[] = { (USB_SELF_POWERED<<0), 0 }; // first byte tells self powered and remote wakeup status

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
       32, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
   {/*Enpoint 1 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       0x03, // bmAttributes
       32, // MaxPacketSize (LITLE ENDIAN)
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


const unsigned char string_descriptor0[] = { // available languages  descriptor
    0x04, STRING_DESCRIPTOR, //
    0x09, 0x04, //
};
// (Standard system devices)
// const unsigned char string_descriptor1[] = { //
//     0x0E, STRING_DESCRIPTOR, // bLength, bDscType
//     'T', 0x00, //
//     'e', 0x00, //
//     's', 0x00, //
//     't', 0x00, //
//     'i', 0x00, //
//     '!', 0x00, //
// };
const unsigned char string_descriptor1[] = { //
    52, STRING_DESCRIPTOR, // bLength, bDscType
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
};
const unsigned char string_descriptor2[] = { //
    0x2E, STRING_DESCRIPTOR, //
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
};
#endif
