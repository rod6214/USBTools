/*
 * File:   usb.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 8:37 PM
 */

#include <xc.h>
#include <string.h>
#include <pic18f2550.h>
#include "usbtypes.h"

// Global variables
BYTE usb_device_state;
BYTE current_configuration; // 0 or 1
BYTE control_stage; // Holds the current stage in a control transfer
BYTE request_handled; // Set to 1 if request was understood and processed.
BYTE device_address;
BYTE dlen; // Number of unsigned chars of data
// BYTE pending_data;
BYTE ep_pending_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
codePtr code_ptr; // Data to host from FLASH
codePtr _devDesc; 
codePtr _configDesc; 
codePtr *_stringDescs;
codePtr _hid_rpt01;


static const char const_values_0x00_0x00[] = { 0, 0 };
volatile USBControlPacket setup_packet __at(USB_TX0_REG);
volatile BYTE ep0_in_buffer[USB_BUFFER_CONTROL_SIZE] __at(USB_RX0_REG);
volatile BYTE ep1_tx_buffer[USB_EP_BUFFER_LEN] __at(USB_TX1_REG);
volatile BYTE ep1_rx_buffer[USB_EP_BUFFER_LEN] __at(USB_RX1_REG);
// volatile BYTE *epx_rx_buffer[] = {
// 	ep0_in_buffer,
// 	ep1_rx_buffer
// };
int usb_sp = 0; // Stack Pointer
BYTE usb_stack[RECEPTOR_LENGTH] __at(RECEPTOR_1_REG);

// *************** Definitions *************** //
#define USBCDC_SELF_POWERED 1
#define flush_data(epx_bd) do {\
if (epx_bd.STAT & DTS)\
		epx_bd.STAT = UOWN | DTSEN;\
	else\
		epx_bd.STAT = UOWN | DTS | DTSEN;\
} while(0)
// #define __wait(epx) while ((epx.STAT & UOWN)!=0) 
// *************** Definitions *************** //

static const char const_values_status[] = { 0, 0 };

//endpoints
volatile BDT ep0_o __at (0x0400+0*8);
volatile BDT ep0_i __at (0x0404+0*8);
volatile BDT ep1_o __at (0x0400+1*8);
volatile BDT ep1_i __at (0x0404+1*8);
volatile BDT ep2_o __at (0x0400+2*8);
volatile BDT ep2_i __at (0x0404+2*8);
volatile BDT ep3_o __at (0x0400+3*8);
volatile BDT ep3_i __at (0x0404+3*8);

static void usb_read_buffer();
static void in_data_stage();
static void prepare_for_setup_stage();
static void get_descriptor();
static void configure_tx_rx_ep();
static BYTE usb_read_ep1_buffer();
static BYTE usb_write_ep1_buffer(BYTE len);
static void get_status(void);



void __wait(int epid, int dir) {
	
	switch (epid)
	{
		case EP1:
		{
			if (dir) {
				while ((ep1_i.STAT & UOWN)!=0);
				return;
			}
			while ((ep1_o.STAT & UOWN)!=0);
		}
		break;
	}
}

BYTE* __get_epx_buffer(int epid, int dir) {
	switch (epid)
	{
		case EP1:
		{
			if (dir) {
				return (BYTE*)ep1_tx_buffer;
			}
			return (BYTE*)ep1_rx_buffer;
		}
		default:
			return 0;
	}
}

void __flush_ep(int epid, int dir, int bytes) {
	switch (epid)
	{
		case EP1:
		{
			if (dir) {
				ep1_i.CNT = bytes;
				flush_data(ep1_i);
			} else {
				flush_data(ep1_o);
			}
		}
		break;
	}
}

int usb_read(int epid, BYTE* buffer, int bytes) {
	if (ep_pending_data[epid] > 0) {
		int idx = 0;
		for (; idx < bytes; idx++) {
			buffer[idx] = __get_epx_buffer(epid, RX)[idx];
		}
		__flush_ep(epid, RX, 0);
		ep_pending_data[epid]--;
		return idx++;
	}
	return 0;
}

int usb_write(int epid, BYTE* buffer, int bytes) {
	int idx = 0;
	__wait(epid, TX);
	for (; idx < bytes; idx++) {
		BYTE *tmp = &(__get_epx_buffer(epid, TX)[idx]);
		*tmp = buffer[idx];
	}
	__flush_ep(epid, TX, bytes);
	return idx++;
}

BYTE get_device_state() {
	return usb_device_state;
}

static void get_status(void) {
	// Mask off the Recipient bits
	unsigned char recipient = (unsigned char)(setup_packet.bmRequestType & 0x1F);
		// See where the request goes
	if (recipient == 0x00) {
		// Device
        request_handled = 1;
		code_ptr = (codePtr) const_values_status; // hard __code device status

	} else if (recipient == 0x01) {
		// Interface
		
	} else if (recipient == 0x02) {
	}
	if (request_handled) {
		dlen = 2;
	}
    
	// // See where the request goes
	// if (recipient == 0x00) {
	// 	// Device
	// 	request_handled = 1;
	// 	code_ptr = (codePtr) const_values_status; // hard __code device status
        
	// } else if (recipient == 0x01) {
	// 	// Interface
	// 	code_ptr = (codePtr) const_values_0x00_0x00;
	// 	request_handled = 1;
	// } else if (recipient == 0x02) {
	// 	// Endpoint
	// 	unsigned char endpointNum = (unsigned char)(setup_packet.windex0 & 0x0F);
	// 	unsigned char endpointDir = (unsigned char)(setup_packet.windex0 & 0x80);
	// 	request_handled = 1;
	// 	// Endpoint descriptors are 8 unsigned chars long, with each in and out taking 4 unsigned chars
	// 	// within the endpoint. (See PIC datasheet.)
	// 	in_ptr = (dataPtr) &ep0_o + (endpointNum * 8);
	// 	if (endpointDir)
	// 		in_ptr += 4;
	// 	if (*in_ptr & BSTALL)
	// 		code_ptr = (codePtr) const_values_0x01_0x00;
	// }
	// if (request_handled) {
	// 	dlen = 2;
	// }
}

static void get_descriptor() {

    BYTE descriptorType = HBYTE(setup_packet.wValue);
    BYTE descriptorIndex = LBYTE(setup_packet.wValue);

	if (setup_packet.bmRequestType == 0x80) {

		if (descriptorType == DEVICE_DESCRIPTOR) {
			
			request_handled = 1;
			code_ptr = (codePtr) _devDesc;
			dlen = *code_ptr;

		} else if (descriptorType == CONFIGURATION_DESCRIPTOR) {

			request_handled = 1;        
			code_ptr = (codePtr) _configDesc;
			dlen = (((Desc_t*)_configDesc)->configDesc).wTotalLength;
            
		} else if (descriptorType == STRING_DESCRIPTOR) {

			request_handled = 1;
			code_ptr = (codePtr)_stringDescs[descriptorIndex];
			dlen = *code_ptr;
		}

	} 
	// else if (setup_packet.bmRequestType == 0x81) {

	// 	if (descriptorType == HID_DESCRIPTOR) {

	// 	} else if (descriptorType == REPORT_DESCRIPTOR) {

	// 		if (!configured_ep && usb_device_state == CONFIGURED) {
	// 			configure_tx_rx_ep();
	// 			configured_ep++;
	// 		}

	// 		request_handled = 1;
	// 		code_ptr = (codePtr) _hid_rpt01;
	// 		dlen = HID_RPT01_SIZE;

	// 	} else if (descriptorType == PHYSICAL_DESCRIPTOR) {
	// 	}
	// }
}

static void configure_tx_rx_ep() {
	// Initialize the endpoints for all interfaces
	{ // Turn on both in and out for this endpoint	
		// UEP1 = 0x1E;
		// ep1_o.CNT = USB_EP_BUFFER_LEN;
		// ep1_o.ADDR = (int) ep1_rx_buffer;
		// ep1_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
		
		// ep1_i.ADDR = (int) ep1_tx_buffer;
		// ep1_i.STAT = 0;
	}
}



static BYTE usb_read_ep1_buffer() {
	ep1_o.CNT = 2;
	if (ep1_o.STAT & DTS)
		ep1_o.STAT = UOWN | DTSEN;
	else
		ep1_o.STAT = UOWN | DTS | DTSEN;
	return USB_EP_BUFFER_LEN;
}

static BYTE usb_write_ep1_buffer(BYTE len) {
	while ((ep1_i.STAT & UOWN) == UOWN);
	// PORTB++;
	ep1_i.STAT &= 0x48;
	ep1_i.CNT = 2;
	if (ep1_i.STAT & DTS)
		ep1_i.STAT |= UOWN | DTSEN;
	else
		ep1_i.STAT |= UOWN | DTS | DTSEN;

	return USB_EP_BUFFER_LEN;
}

static void prepare_for_setup_stage() {
	control_stage = SETUP_STAGE;
	ep0_o.CNT = USB_BUFFER_CONTROL_SIZE;
	ep0_o.ADDR = (int) &setup_packet;
	ep0_o.STAT = UOWN | DTSEN;
	ep0_i.STAT = 0x00;
	UCONbits.PKTDIS = 0;
}

static void in_data_stage() {
	unsigned char bufferSize;
	// Determine how many unsigned chars are going to the host
	if (dlen < USB_BUFFER_CONTROL_SIZE)
		bufferSize = dlen;
	else
		bufferSize = USB_BUFFER_CONTROL_SIZE;
    
	// Load the high two bits of the unsigned char dlen into BC8:BC9
	ep0_i.STAT &= ~(BC8| BC9); // Clear BC8 and BC9
	//ep0_i.STAT |= (unsigned char) ((bufferSize & 0x0300) >> 8);
	//ep0_i.CNT = (unsigned char) (bufferSize & 0xFF);
	ep0_i.CNT = bufferSize;
	ep0_i.ADDR = (int) &ep0_in_buffer[0];
	// Update the number of unsigned chars that still need to be sent.  Getting
	// all the data back to the host can take multiple transactions, so
	// we need to track how far along we are.
	dlen = dlen - bufferSize;
	// memcpy((UINT*)ep0_i.ADDR, code_ptr, bufferSize);
	dataPtr in_ptr = (dataPtr) &ep0_in_buffer[0];
    
	//	for (idx = 0; idx < bufferSize; idx++)
	for (int idx = bufferSize; idx--;)
		*in_ptr++ = *code_ptr++;
}

static void process_control_transfer() {

    if (IS_OUT_EP0) {

		if (IS_SETUP(ep0_o)) {
			
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
			{// ----------
				BYTE request = setup_packet.bRequest;

				if (request == SET_ADDRESS) {
					// Set the address of the device.  All future requests
					// will come to that address.  Can't actually set UADDR
					// to the new address yet because the rest of the SET_ADDRESS
					// transaction uses address 0.     
					request_handled = 1;
					usb_device_state = ADDRESS;
					device_address = LBYTE(setup_packet.wValue);
				} else if (request == GET_DESCRIPTOR) {
					get_descriptor();
				} else if (request == GET_STATUS) {
					get_status();
				} 
				else if (request == SET_CONFIGURATION) {
					
					request_handled = 1;
					current_configuration = LBYTE(setup_packet.wValue);
					// TBD: ensure the new configuration value is one that
					// exists in the descriptor.
					if (current_configuration == 0) {
						// If configuration value is zero, device is put in
						// address state (USB 2.0 - 9.4.7)
						usb_device_state = ADDRESS;
					} else {
						// Set the configuration.
						usb_device_state = CONFIGURED;
						// Initialize the endpoints for all interfaces
						{ // Turn on both in and out for this endpoint
							UEP1 = 0x1E;
							ep1_o.CNT = USB_EP_BUFFER_LEN;
							ep1_o.ADDR = (int) ep1_rx_buffer;
							ep1_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
							
							ep1_i.ADDR = (int) ep1_tx_buffer;
							ep1_i.STAT = DTS;
							// ep1_i.STAT = 0;
							// UEP1 = 0x1E;
							// ep1_i.ADDR = (int) ep1_rx_buffer;
							// ep1_i.STAT = DTS;
						}
					}
				} else if (request == GET_INTERFACE) {
					// No support for alternate interfaces.  Send
					// zero back to the host.
					request_handled = 1;
					code_ptr = (codePtr) (const_values_0x00_0x00);
					dlen = 1;
                } else if (request == SET_INTERFACE) {
				} else if (request == SET_FEATURE) {
				}
			}
            
			if (!request_handled) {
				// If this service wasn't handled then stall endpoint 0
				ep0_o.CNT = USB_BUFFER_CONTROL_SIZE;
				ep0_o.ADDR = (int) &setup_packet;
				ep0_o.STAT = UOWN | BSTALL;
				ep0_i.STAT = UOWN | BSTALL;
			} else if (setup_packet.bmRequestType & 0x80) {
				// Device-to-host
				if (setup_packet.wLength < dlen)//9.4.3, p.253
					dlen = setup_packet.wLength;
                
				in_data_stage();
				control_stage = DATA_IN_STAGE;
				// Reset the out buffer descriptor for endpoint 0
				ep0_o.CNT = USB_BUFFER_CONTROL_SIZE;
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
				ep0_o.CNT = USB_BUFFER_CONTROL_SIZE;
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
				bufferSize = ep0_o.CNT;
                
				// Accumulate total number of unsigned chars read
				dlen = dlen + bufferSize;
                
			}
            
			// Turn control over to the SIE and toggle the data bit
			if (ep0_o.STAT & DTS)
				ep0_o.STAT = UOWN | DTSEN;
			else
				ep0_o.STAT = UOWN | DTS | DTSEN;
		} else {
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
		}
	} else if (IS_IN_EP0) {
		// Endpoint 0:in
		//set address
		if ((UADDR == 0) && (usb_device_state == ADDRESS)) {
			// TBD: ensure that the new address matches the value of
			// "device_address" (which came in through a SET_ADDRESS).
			UADDR = LBYTE(setup_packet.wValue);
			if (UADDR == 0) {
				// If we get a reset after a SET_ADDRESS, then we need
				// to drop back to the Default state.
				usb_device_state = DEFAULT;
			}
		}
        
		if (control_stage == DATA_IN_STAGE) {
			// Start (or continue) transmitting data
			in_data_stage();
			// Turn control over to the SIE and toggle the data bit
			if (ep0_i.STAT & DTS)
				ep0_i.STAT = UOWN | DTSEN;
			else
				ep0_i.STAT = UOWN | DTS | DTSEN;
		} else {
			// Prepare for the Setup stage of a control transfer
			prepare_for_setup_stage();
		}
	} else if (IS_IN_EP1) {
		if (ep_pending_data[EP1] == 0) {
			ep_pending_data[EP1]++;
		}
	}
}

void set_descriptors(codePtr devDesc, codePtr configDesc, codePtr hid_rpt01, codePtr *stringDescs) {
	_devDesc = devDesc; 
    _configDesc = configDesc; 
    _stringDescs = stringDescs;
	_hid_rpt01 = hid_rpt01;
}

void usb_init() {
    
	if (usb_device_state == DETACHED) {
		UCFG = 0x14; // Enable pullup resistors; full speed mode      
		
		current_configuration = 0; 

		// attach
		if (UCONbits.USBEN == 0) {//enable usb controller
			UCON = 0;
			UIE = 0;
			
			UCONbits.USBEN = 1;
			usb_device_state = ATTACHED;
		}
		
		{//Wait for bus reset
			UIEbits.SOFIE = 1;
			UIR = 0;
			UIE = 0;
			UIEbits.URSTIE = 1;
			usb_device_state = POWERED;
		}
		
		PIE2bits.USBIE = 1;
	}
}

void usb_interrupt_handler() {

    if ((UCFGbits.UTEYE == 1) || //eye test
    (usb_device_state == DETACHED) || //not connected
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
			usb_device_state = DEFAULT;
            
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
		UIRbits.TRNIF = 0;
	}
}
