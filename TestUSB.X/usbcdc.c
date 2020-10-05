/*
 File: usbcdc.h
 
 Copyright (c) 2010,2013 Kustaa Nyholm / SpareTimeLabs
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
 Version 1.1     Compatible with SDCC 3.x
 Respond to GET_LINE_CODING to work with overzealous
 Windows software (like Hyperterminal)
 */
#define _XTAL_FREQ 4000000
#include <xc.h>
#include "pic18f2550.h"
#include "usbcdc.h"
#include "usbpic_defs.h"
#include "usbcdc_defs.h"
#include "usb_defs.h"

typedef const unsigned char* codePtr;
typedef unsigned char* dataPtr;

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

// Global variables
unsigned char usbcdc_device_state;

static unsigned char device_address;
static unsigned char current_configuration; // 0 or 1
static unsigned char idx; // loop counter for data transfers loops
static unsigned char control_stage; // Holds the current stage in a control transfer
static unsigned char request_handled; // Set to 1 if request was understood and processed.
static dataPtr data_ptr; // Data to host from RAM
static codePtr code_ptr; // Data to host from FLASH
static dataPtr in_ptr; // Data from the host
static unsigned char dlen; // Number of unsigned chars of data

// See USB spec chapter 5
#define SETUP_STAGE    0
#define DATA_OUT_STAGE 1
#define DATA_IN_STAGE  2
#define STATUS_STAGE   3

const unsigned char device_descriptor[] = { //
    0x12, // bLength
	0x01, // bDescriptorType
    0x00, 0x02, // bcdUSB lsb, bcdUSB msb
    0x00, // bDeviceClass
	0x00, // bDeviceSubClass
    0x00, // bDeviceProtocl
	E0SZ, // bMaxPacketSize
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
    0x00, E0SZ, // bDeviceProtocl, bMaxPacketSize
    0x01, 0x00 // iSerialNumber, bNumConfigurations*/
};

static const char const_values_0x00_0x00[] = { 0, 0 };
static const char const_values_0x00_0x01[] = { 0, 1 };
static const char const_values_0x01_0x00[] = { 1, 0 };
static const char const_values_status[] = { (USBCDC_SELF_POWERED<<0), 0 }; // first byte tells self powered and remote wakeup status

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
       E0SZ, // MaxPacketSize (LITLE ENDIAN)
       1, // bInterval
   },
   {/*Enpoint 1 OUT descriptor*/
       sizeof(EndpointDescriptor_t), // Length
       0x05, // bDescriptorType
       0x01, // bEndpointAddress
       0x03, // bmAttributes
       E0SZ, // MaxPacketSize (LITLE ENDIAN)
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

// Put endpoint 0 buffers into dual port RAM
// Put USB I/O buffers into dual port RAM.
//#pragma udata usbram5 setup_packet control_transfer_buffer cdc_rx_buffer cdc_tx_buffer cdcint_buffer

//static volatile setup_packet_struct setup_packet;
//static volatile unsigned char control_transfer_buffer[E0SZ];

// CDC buffers
static unsigned char tx_len = 0;
static unsigned char rx_idx = 0;

//static volatile unsigned char cdcint_buffer[USBCDC_BUFFER_LEN];
//volatile unsigned char cdc_rx_buffer[USBCDC_BUFFER_LEN];
//volatile unsigned char cdc_tx_buffer[USBCDC_BUFFER_LEN];
//static volatile unsigned char cdcint_buffer[USBCDC_BUFFER_LEN];
static const char cdc_line_coding[7] = {9600&0xFF,9600>>8,0,0,0,0,8};
//static __code char cdc_line_coding[7] = {9600&0xFF,9600>>8,0,0,1,2,8};
void configure_tx_rx_ep() {
	// Initialize the endpoints for all interfaces
	{ // Turn on both in and out for this endpoint
		// UEP1 = 0x1E;
		
		// ep1_i.ADDR = (int) cdcint_buffer;
		// ep1_i.STAT = DTS;
		
		UEP1 = 0x1E;
		
		ep1_o.CNT = sizeof(cdc_rx_buffer);
		ep1_o.ADDR = (int) cdc_rx_buffer;
		ep1_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
		
		ep1_i.ADDR = (int) cdc_tx_buffer;
		ep1_i.STAT = DTS;
	}
}

void usbcdc_putchar(char c)
{
	while (usbcdc_wr_busy());
	cdc_tx_buffer[tx_len++]=c;
	if (tx_len>=sizeof(cdc_tx_buffer)) {
		usbcdc_flush();
	}
}

char usbcdc_wr_busy() {
	return (ep2_i.STAT & UOWN)!=0;
}

unsigned char usbcdc_rd_ready() {
	if (ep2_o.STAT & UOWN)
		return 0;
	if (rx_idx >= ep2_o.CNT) {
		usbcdc_read();
		return 0;
	}
	return 1;
}

void usbcdc_write(unsigned char len)
{
	if (len> 0) {
		ep2_i.CNT = len;
		if (ep2_i.STAT & DTS)
            ep2_i.STAT = UOWN | DTSEN;
		else
            ep2_i.STAT = UOWN | DTS | DTSEN;
	}
}

void usbcdc_flush() {
	usbcdc_write(tx_len);
	tx_len = 0;
    
}

void usbcdc_read() {
	rx_idx=0;
	ep2_o.CNT = sizeof(cdc_rx_buffer);
	if (ep2_o.STAT & DTS)
		ep2_o.STAT = UOWN | DTSEN;
	else
		ep2_o.STAT = UOWN | DTS | DTSEN;
}


char usbcdc_getchar() {
	char c;
	while (!usbcdc_rd_ready());
    
	c = cdc_rx_buffer[rx_idx++];
	if (rx_idx>=ep2_o.CNT) {
		usbcdc_read();
    }
	return c;
}

int configured_ep = 0;

static void get_descriptor(void) {

	unsigned char descriptorType = setup_packet.wvalue1;
	unsigned char descriptorIndex = setup_packet.wvalue0;

	if (setup_packet.bmrequesttype == 0x80) {
		
		if (descriptorType == DEVICE_DESCRIPTOR) {
			
			request_handled = 1;
			code_ptr = (codePtr) device_descriptor;
			dlen = *code_ptr;//DEVICE_DESCRIPTOR_SIZE;
		} else if (descriptorType == QUALIFIER_DESCRIPTOR) {
			// request_handled = 1;
			// code_ptr = (codePtr) device_qualifier_descriptor;
			// dlen = sizeof(device_qualifier_descriptor);
		} else if (descriptorType == CONFIGURATION_DESCRIPTOR) {
			// 
			request_handled = 1;
            
			code_ptr = (codePtr) &config_descriptor;
			// dlen = *(code_ptr + 2);
			dlen = config_descriptor.configDesc.wTotalLength;
			
            
		} else if (descriptorType == STRING_DESCRIPTOR) {
			
			request_handled = 1;
			if (descriptorIndex == 0) {
				code_ptr = (codePtr) string_descriptor0;
			} else if (descriptorIndex == 1) {
				code_ptr = (codePtr) string_descriptor1;
				
			} else if (descriptorIndex == 2) {
				code_ptr = (codePtr) string_descriptor2;
				
			}
			dlen = *code_ptr;
		}

	} else if (setup_packet.bmrequesttype == 0x81) {

		        //  if (usbcdc_device_state == CONFIGURED) {
				// 		PORTB++;
				// 	}

		if (descriptorType == HID_DESCRIPTOR) {
			// PORTB = 1;
			// if (usbcdc_device_state == CONFIGURED) {
			// 			PORTB++;
			// 		}
		} else if (descriptorType == REPORT_DESCRIPTOR) {

			if (!configured_ep && usbcdc_device_state == CONFIGURED) {
				configure_tx_rx_ep();
				configured_ep++;
			}
			        //          if (usbcdc_device_state == CONFIGURED) {
					// 	PORTB++;
					// }

			request_handled = 1;
			code_ptr = (codePtr) &hid_rpt01;
			dlen = HID_RPT01_SIZE;

		} else if (descriptorType == PHYSICAL_DESCRIPTOR) {
			// PORTB = 3;
		}
	}
}

// Process GET_STATUS
static void get_status(void) {
	// Mask off the Recipient bits
	unsigned char recipient = (unsigned char)(setup_packet.bmrequesttype & 0x1F);
    
	// See where the request goes
	if (recipient == 0x00) {
		// Device
		request_handled = 1;
		code_ptr = (codePtr) const_values_status; // hard __code device status
        
	} else if (recipient == 0x01) {
		// Interface
		code_ptr = (codePtr) const_values_0x00_0x00;
		request_handled = 1;
	} else if (recipient == 0x02) {
		// Endpoint
		unsigned char endpointNum = (unsigned char)(setup_packet.windex0 & 0x0F);
		unsigned char endpointDir = (unsigned char)(setup_packet.windex0 & 0x80);
		request_handled = 1;
		// Endpoint descriptors are 8 unsigned chars long, with each in and out taking 4 unsigned chars
		// within the endpoint. (See PIC datasheet.)
		in_ptr = (dataPtr) &ep0_o + (endpointNum * 8);
		if (endpointDir)
			in_ptr += 4;
		if (*in_ptr & BSTALL)
			code_ptr = (codePtr) const_values_0x01_0x00;
	}
	if (request_handled) {
		dlen = 2;
	}
}

// Process SET_FEATURE and CLEAR_FEATURE
static void set_feature(void) {
	unsigned char recipient = (unsigned char)(setup_packet.bmrequesttype & 0x1F);
	unsigned char feature = setup_packet.wvalue0;
    
	if (recipient == 0x02) {
		// Endpoint
		unsigned char endpointNum = (unsigned char)(setup_packet.windex0 & 0x0F);
		unsigned char endpointDir = (unsigned char)(setup_packet.windex0 & 0x80);
		if ((feature == ENDPOINT_HALT) && (endpointNum != 0)) {
			char temp; // FIXME can't we find a global variable
            
			// Halt endpoint (as long as it isn't endpoint 0)
			request_handled = 1;
			// Endpoint descriptors are 8 unsigned chars long, with each in and out taking 4 unsigned chars
			// within the endpoint. (See PIC datasheet.)
			in_ptr = (dataPtr) &ep0_o + (endpointNum * 8);
			if (endpointDir)
				in_ptr += 4;
			// FIXME figure out what this is
			if (setup_packet.brequest == SET_FEATURE)
				temp = 0x84;
			else {
				if (endpointDir == 1)
					temp = 0x00;
				else
					temp = 0x88;
			}
			*in_ptr = temp;
            
		}
	}
}

// Data stage for a Control Transfer that sends data to the host
void in_data_stage(void) {
    
	unsigned char bufferSize;
	// Determine how many unsigned chars are going to the host
	if (dlen < E0SZ)
		bufferSize = dlen;
	else
		bufferSize = E0SZ;
    
	// Load the high two bits of the unsigned char dlen into BC8:BC9
	ep0_i.STAT &= ~(BC8| BC9); // Clear BC8 and BC9
	//ep0_i.STAT |= (unsigned char) ((bufferSize & 0x0300) >> 8);
	//ep0_i.CNT = (unsigned char) (bufferSize & 0xFF);
	ep0_i.CNT = bufferSize;
	ep0_i.ADDR = (int) &control_transfer_buffer[0];
	// Update the number of unsigned chars that still need to be sent.  Getting
	// all the data back to the host can take multiple transactions, so
	// we need to track how far along we are.
	dlen = dlen - bufferSize;
	// Move data to the USB output buffer from wherever it sits now.
	in_ptr = (dataPtr) &control_transfer_buffer[0];
    
	//	for (idx = 0; idx < bufferSize; idx++)
	for (idx = bufferSize; idx--;)
		*in_ptr++ = *code_ptr++;
    
}
#define EPINEN (1 << 1)
#define EPOUTEN (1 << 2)
#define EPHSHK (1 << 4)

void prepare_for_setup_stage(void) {
	// UEP0 = EPINEN | EPOUTEN | EPHSHK;
	control_stage = SETUP_STAGE;
	ep0_o.CNT = E0SZ;
	ep0_o.ADDR = (int) &setup_packet;
	ep0_o.STAT = UOWN | DTSEN;
	ep0_i.STAT = 0x00;
	UCONbits.PKTDIS = 0;
}

void process_control_transfer(void) {
	unsigned char _ep = (((15 << 3) & USTAT) >> 3);
if (usbcdc_device_state == CONFIGURED && _ep == 1) {
			usbcdc_read();
						PORTB = cdc_rx_buffer[1];
					}
	if (USTAT == USTAT_OUT) {

		unsigned char PID = (ep0_o.STAT & 0x3C) >> 2; // Pull PID from middle of BD0STAT
		// 13 times
		
		if (PID == 0x0D) {

			// 7 times
			// Setup stage
			// Note: Microchip says to turn off the UOWN bit on the IN direction as
			// soon as possible after detecting that a SETUP has been received.
			ep0_i.STAT &= ~UOWN;
			ep0_o.STAT &= ~UOWN;
            
			// Initialize the transfer process
			control_stage = SETUP_STAGE;
			request_handled = 0; // Default is that request hasn't been handled
            
			dlen = 0; // No unsigned chars transferred
			// See if this is a standard (as definded in USB chapter 9) request
			if (1 /* (setup_packet.bmrequesttype & 0x60) == 0x00*/) {// ----------
				unsigned char request = setup_packet.brequest;

				if (request == SET_ADDRESS) {
					// Set the address of the device.  All future requests
					// will come to that address.  Can't actually set UADDR
					// to the new address yet because the rest of the SET_ADDRESS
					// transaction uses address 0.
                    
					request_handled = 1;
					usbcdc_device_state = ADDRESS;
					device_address = setup_packet.wvalue0;
				} else if (request == GET_DESCRIPTOR) {

					get_descriptor();
				} else if (request == SET_CONFIGURATION) {
					
					request_handled = 1;
					current_configuration = setup_packet.wvalue0;
					// TBD: ensure the new configuration value is one that
					// exists in the descriptor.
					if (current_configuration == 0) {
						// If configuration value is zero, device is put in
						// address state (USB 2.0 - 9.4.7)
						usbcdc_device_state = ADDRESS;
					} else {
						// Set the configuration.
						usbcdc_device_state = CONFIGURED;
                        
						// Initialize the endpoints for all interfaces
						{ // Turn on both in and out for this endpoint
							UEP1 = 0x1E;
                            
							ep1_i.ADDR = (int) cdcint_buffer;
							ep1_i.STAT = DTS;
                            
							UEP2 = 0x1E;
                            
							ep2_o.CNT = sizeof(cdc_rx_buffer);
							ep2_o.ADDR = (int) cdc_rx_buffer;
							ep2_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
                            
							ep2_i.ADDR = (int) cdc_tx_buffer;
							ep2_i.STAT = DTS;
						}
					}
				} else if (request == GET_CONFIGURATION) { // Never seen in Windows
				                   
					request_handled = 1;
					code_ptr = (codePtr) &const_values_0x00_0x01[current_configuration];
					dlen = 1;
				} else if (request == GET_STATUS) {  // Never seen in Windows
					get_status();
				} else if ((request == CLEAR_FEATURE) || (request
                                                          == SET_FEATURE)) {  // Never seen in Windows
					set_feature();
				} else if (request == GET_INTERFACE) { // Never seen in Windows
					// No support for alternate interfaces.  Send
					// zero back to the host.
                    
					request_handled = 1;
					code_ptr = (codePtr) (const_values_0x00_0x00);
					dlen = 1;
                } else if ((request == SET_INTERFACE) || (request == SET_LINE_CODING) || (request == SET_CONTROL_LINE_STATE)) {
                    
					// No support for alternate interfaces - just ignore.
                    
					request_handled = 1;
				} else if (request == GET_LINE_CODING) {
					code_ptr = (codePtr) (cdc_line_coding);
					dlen = sizeof(cdc_line_coding);
					request_handled = 1;
				}
				else {
					
				}
                
			}
            
            
			if (!request_handled) {
				// If this service wasn't handled then stall endpoint 0
				ep0_o.CNT = E0SZ;
				ep0_o.ADDR = (int) &setup_packet;
				ep0_o.STAT = UOWN | BSTALL;
				ep0_i.STAT = UOWN | BSTALL;
			} else if (setup_packet.bmrequesttype & 0x80) {
				// Device-to-host
				if (setup_packet.wlength < dlen)//9.4.3, p.253
					dlen = setup_packet.wlength;
                
				in_data_stage();
				control_stage = DATA_IN_STAGE;
				// Reset the out buffer descriptor for endpoint 0
				ep0_o.CNT = E0SZ;
				ep0_o.ADDR = (int) &setup_packet;
				ep0_o.STAT = UOWN;
				// Set the in buffer descriptor on endpoint 0 to send data
				// NOT NEEDED ep0_i.ADDR = (int) &control_transfer_buffer;
				// Give to SIE, DATA1 packet, enable data toggle checks
				ep0_i.STAT = UOWN | DTS | DTSEN;
			} else {
				// Host-to-device
				control_stage = DATA_OUT_STAGE;
				// Clear the input buffer descriptor
				ep0_i.CNT = 0;
				ep0_i.STAT = UOWN | DTS | DTSEN;
				// Set the out buffer descriptor on endpoint 0 to receive data
				ep0_o.CNT = E0SZ;
				// ep0_o.ADDR = (int) &control_transfer_buffer[0];
				// ep0_o.ADDR = (int) &setup_packet;;
				// Give to SIE, DATA1 packet, enable data toggle checks
				ep0_o.STAT = UOWN | DTS | DTSEN;
			}
			// Enable SIE token and packet processing
			UCONbits.PKTDIS = 0;
            
		} else if (control_stage == DATA_OUT_STAGE) {
			// Complete the data stage so that all information has
			// passed from host to device before servicing it.
            
			{
				unsigned char bufferSize;
				//bufferSize = ((0x03 & ep0_o.STAT) << 8) | ep0_o.CNT;
				bufferSize = ep0_o.CNT;
                
				// Accumulate total number of unsigned chars read
				dlen = dlen + bufferSize;
				data_ptr = (dataPtr) control_transfer_buffer;
				for (idx = bufferSize; idx--;)
					*in_ptr++ = *data_ptr++;
                
			}
            
			// Turn control over to the SIE and toggle the data bit
			if (ep0_o.STAT & DTS)
				ep0_o.STAT = UOWN | DTSEN;
			else
				ep0_o.STAT = UOWN | DTS | DTSEN;
		} else {
			// 6 times
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
		}
	} else if (USTAT == USTAT_IN) {
		// Endpoint 0:in
        
		// Total times = 1 + 23 = 24
		//set address
		if ((UADDR == 0) && (usbcdc_device_state == ADDRESS)) {
			// 1 time
			// TBD: ensure that the new address matches the value of
			// "device_address" (which came in through a SET_ADDRESS).
			UADDR = setup_packet.wvalue0;
			if (UADDR == 0) {
				// If we get a reset after a SET_ADDRESS, then we need
				// to drop back to the Default state.
				usbcdc_device_state = DEFAULT;
			}
		}
        
		if (control_stage == DATA_IN_STAGE) {
			
			// 23 times
			// Start (or continue) transmitting data
			in_data_stage();
			// Turn control over to the SIE and toggle the data bit
			if (ep0_i.STAT & DTS)
				ep0_i.STAT = UOWN | DTSEN;
			else
				ep0_i.STAT = UOWN | DTS | DTSEN;
		} else {
			// 1 time
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
		}
	}
}

void usbcdc_init() {
	UCFG = 0x14; // Enable pullup resistors; full speed mode
//    UCFG = UPUEN; // Important: for HID must be low speed
	usbcdc_device_state = DETACHED;
	//	remote_wakeup = 0x00;
	current_configuration = 0x00;            
    
	// attach
	if (UCONbits.USBEN == 0) {//enable usb controller
		UCON = 0;
		UIE = 0;
        
		UCONbits.USBEN = 1;
		usbcdc_device_state = ATTACHED;
        
	}
    
	{//Wait for bus reset
		UIR = 0;
		UIE = 0;
		UIEbits.URSTIE = 1;
		usbcdc_device_state = POWERED;
	}
    
	PIE2bits.USBIE = 1;
}

// Main entry point for USB tasks.  Checks interrupts, then checks for transactions.
void usbcdc_handler(void) {
	if ((UCFGbits.UTEYE == 1) || //eye test
        (usbcdc_device_state == DETACHED) || //not connected
        (UCONbits.SUSPND == 1))//suspended
        return;

	// Process a bus reset
	if (UIRbits.URSTIF && UIEbits.URSTIE) {
		{ // bus_reset

			UEIR = 0x00;
			UIR = 0x00;
			UEIE = 0x9f;
			UIE = 0x2b;
			UADDR = 0x00;
            
			// Set endpoint 0 as a control pipe
			UEP0 = 0x16;
            
			// Flush any pending transactions
			while (UIRbits.TRNIF == 1)
				UIRbits.TRNIF = 0;
            
			// Enable packet processing
			UCONbits.PKTDIS = 0;
            
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
            
			current_configuration = 0; // Clear active configuration
			usbcdc_device_state = DEFAULT;
            
		}
		UIRbits.URSTIF = 0;
	}
	//nothing is done to start of frame
	if (UIRbits.SOFIF && UIEbits.SOFIE) {
		UIRbits.SOFIF = 0;
	}
    
	// stall processing
	if (UIRbits.STALLIF && UIEbits.STALLIE) {
		if (UEP0bits.EPSTALL == 1) {
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
			UEP0bits.EPSTALL = 0;
		}
		UIRbits.STALLIF = 0;
	}
	if (UIRbits.UERRIF && UIEbits.UERRIE) {
		// Clear errors
		UIRbits.UERRIF = 0;
	}
    
	// A transaction has finished.  Try default processing on endpoint 0.
	if (UIRbits.TRNIF && UIEbits.TRNIE) {

		process_control_transfer();
		// Turn off interrupt
		// UIRbits.TRNIF = 0;
		while (UIRbits.TRNIF == 1) {
			UIRbits.TRNIF = 0;
		}
	}
}
