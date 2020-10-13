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
codePtr code_ptr; // Data to host from FLASH
codePtr _devDesc; 
codePtr _configDesc; 
codePtr *_stringDescs;
codePtr _hid_rpt01;
int configured_ep = 0;

static const char const_values_0x00_0x00[] = { 0, 0 };
volatile USBControlPacket setup_packet __at(USB_TX0_REG);
volatile BYTE ep0_in_buffer[USB_BUFFER_CONTROL_SIZE] __at(USB_RX0_REG);
volatile BYTE ep1_tx_buffer[USB_EP_BUFFER_LEN] __at(USB_TX1_REG);
volatile BYTE ep1_rx_buffer[USB_EP_BUFFER_LEN] __at(USB_RX1_REG);
int usb_sp = 0; // Stack Pointer
BYTE usb_stack[2*RECEPTOR_LENGTH] __at(RECEPTOR_0_REG);

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
static void process_interrupt();
static void prepare_for_setup_stage();
static void get_descriptor();
static void configure_tx_rx_ep();
static BYTE usb_read_ep1_buffer() ;

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

	} else if (setup_packet.bmRequestType == 0x81) {

		if (descriptorType == HID_DESCRIPTOR) {

		} else if (descriptorType == REPORT_DESCRIPTOR) {

			if (!configured_ep && usb_device_state == CONFIGURED) {
				configure_tx_rx_ep();
				configured_ep++;
			}

			request_handled = 1;
			code_ptr = (codePtr) _hid_rpt01;
			dlen = HID_RPT01_SIZE;

		} else if (descriptorType == PHYSICAL_DESCRIPTOR) {
		}
	}
}

static void configure_tx_rx_ep() {
	// Initialize the endpoints for all interfaces
	{ // Turn on both in and out for this endpoint	
		UEP1 = 0x1E;
		ep1_o.CNT = USB_EP_BUFFER_LEN;
		ep1_o.ADDR = (int) ep1_rx_buffer;
		ep1_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
		
		ep1_i.ADDR = (int) ep1_tx_buffer;
		ep1_i.STAT = DTS;
	}
}

static void usb_read_buffer() {
	ep1_o.CNT = USB_EP_BUFFER_LEN;
	if (ep1_o.STAT & DTS)
		ep1_o.STAT = UOWN | DTSEN;
	else
		ep1_o.STAT = UOWN | DTS | DTSEN;
}

static BYTE usb_read_ep1_buffer() {
	ep1_o.CNT = USB_EP_BUFFER_LEN;
	if (ep1_o.STAT & DTS)
		ep1_o.STAT = UOWN | DTSEN;
	else
		ep1_o.STAT = UOWN | DTS | DTSEN;
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

BYTE testing __at(0x800);

int prepare_ready_ep1 = 0;

static void process_interrupt() {
	if (!configured_ep && usb_device_state == CONFIGURED) {
		configure_tx_rx_ep();
		configured_ep++;
	}
	// This comment works fine receiving data from host with interrupt transaction
	if (usb_device_state == CONFIGURED) {
		if (usb_sp >= 0 && usb_sp < 512) {
			if (IS_IN_EP1) {
				BYTE bytes = usb_read_ep1_buffer();
				if (prepare_ready_ep1 == 2) {

					memcpy((void*)&usb_stack[usb_sp], (void*)&ep1_rx_buffer[0], bytes);

					usb_sp += bytes;
				}
				else {
					prepare_ready_ep1++;
				}
			}
		}
	}
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
				} else if (request == SET_CONFIGURATION) {
					
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
							ep1_i.ADDR = (int) ep1_rx_buffer;
							ep1_i.STAT = DTS;
						}
					}
				} else if (request == GET_INTERFACE) {
					// No support for alternate interfaces.  Send
					// zero back to the host.
					request_handled = 1;
					code_ptr = (codePtr) (const_values_0x00_0x00);
					dlen = 1;
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
		// PORTB++;
		// PORTA++;
		process_control_transfer();
		process_interrupt();
		// Turn off interrupt
		UIRbits.TRNIF = 0;
	}
}
