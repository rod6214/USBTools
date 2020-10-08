/*
 * File:   usb.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 8:37 PM
 */


#include <xc.h>
#include <string.h>
#include <pic18f2550.h>
#include "../types/usbtypes.h"
#include "../definitions/usb.h"

#define MAX_BUFFER_SIZE 64

// Global variables
static BYTE usbcdc_device_state;
static unsigned char current_configuration; // 0 or 1
static unsigned char control_stage; // Holds the current stage in a control transfer
static unsigned char request_handled; // Set to 1 if request was understood and processed.
static unsigned char device_address;
static unsigned char dlen; // Number of unsigned chars of data
static codePtr code_ptr; // Data to host from FLASH

void usb_init(USB* pUsb);

static void get_descriptor() {}

static void prepare_for_setup_stage() {}

static void in_data_stage() {}

static void process_control_transfer() {

    if (IS_OUT_EP0) {

		// unsigned char PID = (ep0_o.STAT & 0x3C) >> 2; // Pull PID from middle of BD0STAT
		
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
				}
			}
            
			if (!request_handled) {
				// If this service wasn't handled then stall endpoint 0
				ep0_o.CNT = MAX_BUFFER_SIZE;
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
				ep0_o.CNT = MAX_BUFFER_SIZE;
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
				ep0_o.CNT = MAX_BUFFER_SIZE;
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
	} else if (IS_IN_EP0) {
		// Endpoint 0:in
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

USB* create_usb(Desc_t* desc) {
	usb_init(0);
}

void usb_init(USB* pUsb) {
	if (pUsb->usb_device_state == DETACHED) {
		UCFG = 0x14; // Enable pullup resistors; full speed mode
	//    UCFG = UPUEN; // Important: for HID must be low speed
		// usbcdc_device_state = DETACHED;
		//	remote_wakeup = 0x00;
		current_configuration = 0x00;            
		
		// attach
		if (UCONbits.USBEN == 0) {//enable usb controller
			UCON = 0;
			UIE = 0;
			
			UCONbits.USBEN = 1;
			pUsb->usb_device_state = ATTACHED;
			
		}
		
		{//Wait for bus reset
			UIR = 0;
			UIE = 0;
			UIEbits.URSTIE = 1;
			pUsb->usb_device_state = POWERED;
		}
		
		PIE2bits.USBIE = 1;
	}
}

void usb_interrupt_handler() {
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
