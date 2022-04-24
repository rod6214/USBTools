
#if __USB__
// #define _XTAL_FREQ 4000000
#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include "pic18f2550.h"
#include "usb.h"
#include "usbpic_defs.h"
#include "usb_defs.h"
#include "cstream.h"

#define RAM_BUFFER_BASE 0x500
#define SETUP_PACKET_REG RAM_BUFFER_BASE
volatile setup_packet_struct setup_packet __at(SETUP_PACKET_REG);

#define CONTROL_TRANSFER_REG (SETUP_PACKET_REG + ENDPOINT_0_SIZE + sizeof(setup_packet))
#define TX_REG (CONTROL_TRANSFER_REG + ENDPOINT_0_SIZE)
#define RX_REG (TX_REG + USB_BUFFER_LEN)

volatile unsigned char control_transfer_buffer[ENDPOINT_0_SIZE] __at(CONTROL_TRANSFER_REG);
volatile unsigned char tx_buffer[USB_BUFFER_LEN] __at(TX_REG);
volatile unsigned char rx_buffer[USB_BUFFER_LEN] __at(RX_REG);

struct USBHandler UPtr;

typedef const unsigned char* codePtr;
typedef unsigned char* dataPtr;

// Global variables
unsigned char usb_device_state;
static unsigned char device_address;
static unsigned char current_configuration; // 0 or 1
static unsigned char idx; // loop counter for data transfers loops
static unsigned char control_stage; // Holds the current stage in a control transfer
static unsigned char request_handled; // Set to 1 if request was understood and processed.
static dataPtr data_ptr; // Data to host from RAM
static codePtr code_ptr; // Data to host from FLASH
static dataPtr in_ptr; // Data from the host
static unsigned int dlen; // Number of unsigned chars of data

// See USB spec chapter 5
#define SETUP_STAGE    0
#define DATA_OUT_STAGE 1
#define DATA_IN_STAGE  2
#define STATUS_STAGE   3

#define EPINEN (1 << 1)
#define EPOUTEN (1 << 2)
#define EPHSHK (1 << 4)

#include "descriptor.h"

// Bytes transmited
static int tx_len = 0;
// Rx pointer
static int rx_idx = 0;
// Stream pointer
static Stream_t stream;
// Flag to check when to send the completion package
int _pending = TRUE;

static size_t _usb_read();
static void _usb_flush();
static void _usb_write(int len);
static unsigned char _usb_wr_busy();
static unsigned char _usb_rd_ready();

void* usb_getStream() 
{
	stream.type = USB_STREAM;
	stream.index = 0;
	return &stream;
}

void configure_tx_rx_ep() 
{
	// Initialize the endpoints for all interfaces
	{ // Turn on both in and out for this endpoint
		UEP1 = 0x1E;
		// OUT endpoint host to device.
		ep1_o.CNT = sizeof(rx_buffer);
		ep1_o.ADDR = (unsigned int)rx_buffer;
		ep1_o.STAT = UOWN | DTSEN; //set up to receive stuff as soon as we get something
		// IN endpoint device to host.
		ep1_i.ADDR = (unsigned int)tx_buffer;
		ep1_i.STAT = DTS;
	}
}

//int test = 0;

int usb_putchar(char c)
{
	tx_buffer[tx_len++]=c;
    
    if (tx_len > stream.length - 1) 
    {
        
        tx_buffer[tx_len]= '\0';
        _usb_flush();
        return TRUE;
    }
    
    return FALSE;
}

char usb_getchar()
{
	char c = 0;
    size_t len = strlen((char*)rx_buffer);
	if (rx_idx < len) {
		c = rx_buffer[rx_idx++];
	}
	else 
	{
		UPtr.Length = _usb_read();
	}
	return c;
}

void usb_reset_buffers() 
{
    memset((void*)rx_buffer, 0, USB_BUFFER_LEN);
    memset((void*)tx_buffer, 0, USB_BUFFER_LEN);
}

void usb_rewind() {
	rx_idx = 0;
    tx_len = 0;
}

void usb_write(BYTE* data, size_t length) 
{
	memcpy((void*)tx_buffer, data, length);
	_usb_write((unsigned char)length);
}

static void _usb_flush() 
{
	_usb_write(USB_BUFFER_LEN);
	tx_len = 0;
}

static size_t _usb_read() {
	rx_idx=0;
    size_t dataReceived = 0;

    dataReceived = sizeof(rx_buffer);
	ep1_o.CNT = sizeof(rx_buffer);
	if (ep1_o.STAT & DTS)
		ep1_o.STAT = UOWN | DTSEN;
	else
		ep1_o.STAT = UOWN | DTS | DTSEN;
    UPtr.read = FALSE;
    return dataReceived;
}

//static unsigned char _usb_wr_busy() {
//	return (unsigned char)((ep1_i.STAT & UOWN)!=0);
//}
//
//static unsigned char _usb_rd_ready() {
//	return (unsigned char)((ep1_o.STAT & UOWN) == 0);
//}

static void _usb_write(int len)
{
	if (len > 0) {
        
		ep1_i.CNT = (char)len;
		if (ep1_i.STAT & DTS)
			ep1_i.STAT = UOWN | DTSEN;
		else
			ep1_i.STAT = UOWN | DTS | DTSEN;
	}
}

//int usb_ready() 
//{
//   return  ep1_i.STAT & UOWN != UOWN;
//}

int configured_ep = 0;

static void get_descriptor(void) {

	unsigned char descriptorType = setup_packet.wvalue1;
	unsigned char descriptorIndex = setup_packet.wvalue0;

	if (setup_packet.bmrequesttype == 0x80) {
		
		if (descriptorType == DEVICE_DESCRIPTOR) {
			
			request_handled = 1;
			code_ptr = (codePtr) device_descriptor;
			dlen = *code_ptr;//DEVICE_DESCRIPTOR_SIZE;
		} else if (descriptorType == CONFIGURATION_DESCRIPTOR) {
			// 
			request_handled = 1;
			code_ptr = (codePtr) &config_descriptor;
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

		if (descriptorType == HID_DESCRIPTOR) {
		} 
		else if (descriptorType == REPORT_DESCRIPTOR) {
			if (!configured_ep && usb_device_state == CONFIGURED) {
				configure_tx_rx_ep();
				configured_ep++;
			}
			
			request_handled = 1;
			code_ptr = (codePtr) &hid_rpt01;
			dlen = HID_RPT01_SIZE;

		} 
		else if (descriptorType == PHYSICAL_DESCRIPTOR) {
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
	if (dlen < ENDPOINT_0_SIZE)
		bufferSize = (unsigned char)dlen;
	else
		bufferSize = ENDPOINT_0_SIZE;
    
	// Load the high two bits of the unsigned char dlen into BC8:BC9
	ep0_i.STAT &= ~(BC8| BC9); // Clear BC8 and BC9
	//ep0_i.STAT |= (unsigned char) ((bufferSize & 0x0300) >> 8);
	//ep0_i.CNT = (unsigned char) (bufferSize & 0xFF);
	ep0_i.CNT = bufferSize;
	ep0_i.ADDR = (unsigned int) &control_transfer_buffer[0];
	// Update the number of unsigned chars that still need to be sent.  Getting
	// all the data back to the host can take multiple transactions, so
	// we need to track how far along we are.
	dlen = (unsigned char)(dlen - bufferSize);
	// Move data to the USB output buffer from wherever it sits now.
	in_ptr = (dataPtr) &control_transfer_buffer[0];
    
	//	for (idx = 0; idx < bufferSize; idx++)
	for (idx = bufferSize; idx--;)
		*in_ptr++ = *code_ptr++;
    
}

void prepare_for_setup_stage(void) {
	// UEP0 = EPINEN | EPOUTEN | EPHSHK;
	control_stage = SETUP_STAGE;
	ep0_o.CNT = ENDPOINT_0_SIZE;
	ep0_o.ADDR = (unsigned int) &setup_packet;
	ep0_o.STAT = UOWN | DTSEN;
	ep0_i.STAT = 0x00;
	UCONbits.PKTDIS = 0;
}

static unsigned char get_endpoint_processed() 
{
	unsigned char _ep = (unsigned char)(((15 << 3) & USTAT) >> 3);
	return _ep;
}

void process_control_transfer(void) 
{
//    PORTB++;
    UPtr.read = TRUE;
	unsigned char _ep = get_endpoint_processed();
	if (usb_device_state == CONFIGURED && _ep == 1) 
	{
		UPtr.Status = DATA_RECEIVED;
		// UPtr.Length = _usb_read();
	}

	if (USTAT == USTAT_OUT) {

		unsigned char PID = (unsigned char)((ep0_o.STAT & 0x3C) >> 2); // Pull PID from middle of BD0STAT
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
					usb_device_state = ADDRESS;
					UPtr.Status = ADDRESS;
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
						usb_device_state = ADDRESS;
						UPtr.Status = ADDRESS;
					} else {
						// Set the configuration.
						usb_device_state = CONFIGURED;
						UPtr.Status = CONFIGURED;
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
				}
				else {
					
				}
                
			}
            
			if (!request_handled) {
				// If this service wasn't handled then stall endpoint 0
				ep0_o.CNT = ENDPOINT_0_SIZE;
				ep0_o.ADDR = (unsigned int) &setup_packet;
				ep0_o.STAT = UOWN | BSTALL;
				ep0_i.STAT = UOWN | BSTALL;
			} else if (setup_packet.bmrequesttype & 0x80) {
				// Device-to-host
				if (setup_packet.wlength < dlen)//9.4.3, p.253
					dlen = setup_packet.wlength;
                
				in_data_stage();
				control_stage = DATA_IN_STAGE;
				// Reset the out buffer descriptor for endpoint 0
				ep0_o.CNT = ENDPOINT_0_SIZE;
				ep0_o.ADDR = (unsigned int) &setup_packet;
				ep0_o.STAT = UOWN;
				// Give to SIE, DATA1 packet, enable data toggle checks
				ep0_i.STAT = UOWN | DTS | DTSEN;
			} else {
				// Host-to-device
				control_stage = DATA_OUT_STAGE;
				// Clear the input buffer descriptor
				ep0_i.CNT = 0;
				ep0_i.STAT = UOWN | DTS | DTSEN;
				// Set the out buffer descriptor on endpoint 0 to receive data
				ep0_o.CNT = ENDPOINT_0_SIZE;
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
				dlen = (unsigned char)(dlen + bufferSize);
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
		if ((UADDR == 0) && (usb_device_state == ADDRESS)) {
			// 1 time
			// TBD: ensure that the new address matches the value of
			// "device_address" (which came in through a SET_ADDRESS).
			UADDR = setup_packet.wvalue0;
			if (UADDR == 0) {
				// If we get a reset after a SET_ADDRESS, then we need
				// to drop back to the Default state.
				usb_device_state = DEFAULT;
				UPtr.Status = DEFAULT;
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

#define usb_as_lowpriority() (IPR2 = (IPR2)|(1 << 5)&(~(1 << 5)));
#define usb_as_highpriority() (IPR2 = (IPR2)|(1 << 5));

void usb_init() {
	UCFG = 0x14; // Enable pullup resistors; full speed mode
//    UCFG = UPUEN; // Important: for HID must be low speed
	usb_device_state = DETACHED;
	UPtr.Status = DETACHED;
	//	remote_wakeup = 0x00;
	current_configuration = 0x00;            
    
	// attach
	if (UCONbits.USBEN == 0) {//enable usb controller
		UCON = 0;
		UIE = 0;
        
		UCONbits.USBEN = 1;
		usb_device_state = ATTACHED;
		UPtr.Status = ATTACHED;
	}
    
	{//Wait for bus reset
		UIR = 0;
		UIE = 0;
		UIEbits.URSTIE = 1;
		usb_device_state = POWERED;
		UPtr.Status = POWERED;
	}
    
	PIE2bits.USBIE = 1;
}

// Main entry point for USB tasks.  Checks interrupts, then checks for transactions.
void* usb_handler(void) {
	UPtr.Length = 0;
    UPtr.pRxBuffer = (void*)rx_buffer;
    UPtr.pTxBuffer = (void*)tx_buffer;
	if ((UCFGbits.UTEYE == 1) || //eye test
        (usb_device_state == DETACHED) || //not connected
        (UCONbits.SUSPND == 1))//suspended
        return NULL;

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
			UPtr.Status = DEFAULT;
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
		UPtr.Status = DATA_PROCESSING;
        
		process_control_transfer();
		// Turn off interrupt
		// UIRbits.TRNIF = 0;
		while (UIRbits.TRNIF == 1) {
			UIRbits.TRNIF = 0;
		}
	}

	return &UPtr;
}

void usb_SetUsbAsHighPriority() 
{
    // High interrupt disable
    di();
    // USB as priority level
    IPR2bits.USBIP = 1;
    // Interrupt priority enable
    RCONbits.IPEN = 1;
    // Low interrupt disable
    INTCONbits.PEIE = 0;
    // USB interrupt enable
    PIE2bits.USBIE = 1;
    // High interrupt enable
    ei();
}

#endif
