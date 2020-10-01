/*
 * File:   usb.c
 * Author: NelsonAmador
 *
 * Created on July 20, 2020, 8:37 PM
 */


#include <xc.h>
#include <string.h>
#include <pic18f2550.h>
#include "usb.h"
#include "global.h"
#include "descriptors.h"

#define MAX_BUFFER_SIZE 0x2ff

static char table[1024] __at(0x400);
// Buffer descriptor endpoint 0 OUT
volatile BDnSTAT_t BD0STAT __at(0x400);
// Buffer descriptor endpoint 0 IN
volatile BDnSTAT_t BD1STAT __at(0x404);
volatile unsigned char BUFFER0[64] __at(0x500);
volatile unsigned char BUFFER1[64] __at(0x540);
//volatile char BUFFER __at(0x500);
static unsigned char request_handled; // Set to 1 if request was understood and processed.
static unsigned char control_stage; // Holds the current stage in a control transfer
static unsigned int dlen; // Number of unsigned chars of data
//int __memCounter = 0;
char __transaction = 0;

#define SETUP_STAGE    0
#define DATA_OUT_STAGE 1
#define DATA_IN_STAGE  2
#define STATUS_STAGE   3

static void preferredConfig(BYTE package);
void flags();

//void USB_enumeration() {
//    if (BD0STAT.UOWN) {
//        PORTB = 1;
//    }
//}

int ff = 1;

void __interrupt(low_priority) genIntL(void) {
    // PORTBbits.RB1 = 1;
    return;
}

static void get_descriptor(void) {
    USBRequest* setup_packet = (USBRequest*)(&BUFFER0[0]);

	if (setup_packet->bmRequestType == 0x80) {
		unsigned char descriptorType = HIGHBYTE(setup_packet->wValue);
//		unsigned char descriptorIndex = setup_packet->wvalue0;
		if (descriptorType == DEVICE_DESCRIPTOR) {
			request_handled = 1;
            dlen = setup_packet->wLength;
            PORTB++;
            // load_descriptor(&(BUFFER0[0]), deviceDescriptor);
			// code_ptr = (codePtr) &device_descriptor;
			// dlen = *code_ptr;//DEVICE_DESCRIPTOR_SIZE;
		} else if (descriptorType == QUALIFIER_DESCRIPTOR) {
			// request_handled = 1;
			// code_ptr = (codePtr) &device_qualifier_descriptor;
			// dlen = sizeof(device_qualifier_descriptor);
		} else if (descriptorType == CONFIGURATION_DESCRIPTOR) {
            // request_handled = 1;
            // PORTB++;
			// code_ptr = (codePtr) &config_descriptor;
			// dlen = *(code_ptr + 2);
            
		} else if (descriptorType == STRING_DESCRIPTOR) {
            // PORTB++;
            // request_handled = 1;
			// if (descriptorIndex == 0) {
			// 	code_ptr = (codePtr) &string_descriptor0;
			// } else if (descriptorIndex == 1) {
			// 	code_ptr = (codePtr) &string_descriptor1;
			// } else {
			// 	code_ptr = (codePtr) &string_descriptor2;
			// }
			// dlen = *code_ptr;
		}
	}
}

static int counter = 0;

#define EP_CTRL 6
#define HSHK_EN 16

void prepare_for_setup_stage(void) {
    USBRequest* setup_packet = (USBRequest*)(&BUFFER0[0]);
	control_stage = SETUP_STAGE;
	ep0_o.CNT = E0SZ;
	ep0_o.ADDR = (unsigned int) setup_packet;
	ep0_o.STAT = __UOWN | __DTSEN;
	ep0_i.STAT = 0x00;
	UCONbits.PKTDIS = 0;
}

// Data stage for a Control Transfer that sends data to the host
void in_data_stage(void) {
    load_descriptor(&(BUFFER1[0]), &deviceDescriptor);

    // Load the high two bits of the unsigned char dlen into BC8:BC9
	ep0_i.STAT = 0; // Clear BC8 and BC9
	//ep0_i.STAT |= (unsigned char) ((bufferSize & 0x0300) >> 8);
	//ep0_i.CNT = (unsigned char) (bufferSize & 0xFF);
	ep0_i.CNT = 18;
	ep0_i.ADDR = (int) &(BUFFER1[0]);
}

static int addcount = 0;
// static int desccount = 0;

void _test02 () {
    if (USBIF) {
        
        if(STALLIF) {
            if(UEP0bits.EPSTALL)
            {
                UEP0bits.EPSTALL = 0;
            }
            UIRbits.STALLIF = 0;
        }
        
        if (ACTVIF) {
            SUSPND = 0;
            ACTVIF = 0;
        }
        
        if(UERRIF) {
            UERRIF = 0;
        }
        
        if (SOFIF) {
            SOFIF = 0;
        }

        if(URSTIF) {
            
            while(UIRbits.TRNIF)            // Flush any pending transactions
            {
                UIRbits.TRNIF = 0;
                
            }
            UIR   = 0;                      // Clears all USB interrupts
            UIE   = 0x6B;                   // Enable all interrupts except ACTVIE & IDLE
            UADDR = 0;                      // Reset to default address
            UEP0  = EP_CTRL | HSHK_EN;      // Init EP0 as a Ctrl EP
            UCONbits.PKTDIS = 0;            // Make sure packet processing is enabled
            URSTIF = 0;
        }

        if (TRNIF) {
            if (USTAT == DIR_OUT) {
                unsigned char PID = (unsigned char)((ep0_o.STAT & 0x3C) >> 2); // Pull PID from middle of BD0STAT

                if (PID == SETUP) {
                    USBRequest* setup_packet = (USBRequest*)(&BUFFER0[0]);
                    // Setup stage
                    // Note: Microchip says to turn off the UOWN bit on the IN direction as
                    // soon as possible after detecting that a SETUP has been received.
                    ep0_i.STAT &= ~__UOWN;
                    ep0_o.STAT &= ~__UOWN;
                    
                    // Initialize the transfer process
                    control_stage = SETUP_STAGE;
                    request_handled = 0; // Default is that request hasn't been handled
                    
                    // dlen = 0; // No unsigned chars transferred
                    // See if this is a standard (as definded in USB chapter 9) request
                    
                    // debug=setup_packet.bmRequestType;
                    if (1 /* (setup_packet.bmrequesttype & 0x60) == 0x00*/) {// ----------
                        unsigned char request = setup_packet->bRequest;
                        // debug = request;
                        
                        
                        if (request == SET_ADDRESS) {
                            
                            if (!addcount) {
                                addcount++;
                            }
                            // request_handled = 1;

                        } else if (request == GET_DESCRIPTOR) {
                            // if (!addcount) {
                            //     PORTB++;
                            // }
                            // PORTB++;
                            // request_handled = 1;
                            get_descriptor();
                            // load_descriptor(&(BUFFER1[0]), deviceDescriptor);

                        } else if (request == SET_CONFIGURATION) {
                            
                            // PORTB++;
                            // request_handled = 1;
                            
                        } else if (request == GET_CONFIGURATION) { // Never seen in Windows
                            // PORTB++;
                            // request_handled = 1;

                        } else if (request == GET_STATUS) {  // Never seen in Windows
                            // PORTB++;
                            // request_handled = 1;

                        } else if ((request == CLEAR_FEATURE) || (request == SET_FEATURE)) {  // Never seen in Windows
                            // PORTB++;
                            // request_handled = 1;

                        } else if (request == GET_INTERFACE) { // Never seen in Windows
                            // PORTB++;
                            // request_handled = 1;
                            
                        } else if ((request == SET_INTERFACE) || (request == SET_LINE_CODING) || (request == SET_CONTROL_LINE_STATE)) {
                            // No support for alternate interfaces - just ignore.
                            // PORTB++;
                            // request_handled = 1;

                        } else if (request == GET_LINE_CODING) {
                            // PORTB++;
                            // request_handled = 1;
                        }
                        
                    }
                    
                    
                    if (!request_handled) {
                        //  PORTB++;
                        // PORTB = 7;
                        // If this service wasn't handled then stall endpoint 0
                        ep0_o.CNT = E0SZ;
                        ep0_o.ADDR = (unsigned int)setup_packet;
                        ep0_o.STAT = __UOWN | __BSTALL;
                        ep0_i.STAT = __UOWN | __BSTALL;
                    } else if (setup_packet->bmRequestType & 0x80) {
                        // PORTB = 1;
                        // Device-to-host
                        if (setup_packet->wLength < dlen)//9.4.3, p.253
                        	dlen = setup_packet->wLength;
                        in_data_stage();
                        control_stage = DATA_IN_STAGE;
                        // Reset the out buffer descriptor for endpoint 0
                        ep0_o.CNT = E0SZ;
                        ep0_o.ADDR = (int)setup_packet;
                        ep0_o.STAT = __UOWN;
                        // Set the in buffer descriptor on endpoint 0 to send data
                        // NOT NEEDED ep0_i.ADDR = (int) &control_transfer_buffer;
                        // Give to SIE, DATA1 packet, enable data toggle checks
                        ep0_i.STAT = __UOWN | __DTS | __DTSEN;
                    } else {
                        
                        // PORTB = 2;
                        // Host-to-device
                        control_stage = DATA_OUT_STAGE;
                        // Clear the input buffer descriptor
                        ep0_i.CNT = 0;
                        ep0_i.STAT = __UOWN | __DTS | __DTSEN;
                        // Set the out buffer descriptor on endpoint 0 to receive data
                        ep0_o.CNT = E0SZ;
                        ep0_o.ADDR = (int) &BUFFER1[0];
                        // Give to SIE, DATA1 packet, enable data toggle checks
                        ep0_o.STAT = __UOWN | __DTS | __DTSEN;
                    }
                    // Enable SIE token and packet processing
                    UCONbits.PKTDIS = 0;
                    
                } else if (control_stage == DATA_OUT_STAGE) {
                    
                    // Complete the data stage so that all information has
                    // passed from host to device before servicing it.
                    
                    // {
                        
                    // 	unsigned char bufferSize;
                    // 	//bufferSize = ((0x03 & ep0_o.STAT) << 8) | ep0_o.CNT;
                    // 	bufferSize = ep0_o.CNT;
                        
                    // 	// Accumulate total number of unsigned chars read
                    // 	dlen = dlen + bufferSize;
                    // 	data_ptr = (dataPtr) &control_transfer_buffer;
                    // 	for (idx = bufferSize; idx--;)
                    // 		*in_ptr++ = *data_ptr++;
                        
                    // }
                    
                    // Turn control over to the SIE and toggle the data bit
                    if (ep0_o.STAT & __DTS)
                        ep0_o.STAT = __UOWN | __DTSEN;
                    else
                        ep0_o.STAT = __UOWN | __DTS | __DTSEN;
                }
                else {
                    // Prepare for the Setup stage of a control transfer
                    prepare_for_setup_stage();
                }
            } else if (USTAT == DIR_IN) {
                // // Endpoint 0:in
                
                // //set address
                // if ((UADDR == 0) && (usbcdc_device_state == ADDRESS)) {
                // 	// TBD: ensure that the new address matches the value of
                // 	// "device_address" (which came in through a SET_ADDRESS).
                // 	UADDR = setup_packet.wvalue0;
                // 	if (UADDR == 0) {
                // 		// If we get a reset after a SET_ADDRESS, then we need
                // 		// to drop back to the Default state.
                // 		usbcdc_device_state = DEFAULT;
                // 	}
                // }
                
                if (control_stage == DATA_IN_STAGE) {
                    // Start (or continue) transmitting data
                    in_data_stage();
                    // Turn control over to the SIE and toggle the data bit
                    if (ep0_i.STAT & __DTS)
                        ep0_i.STAT = __UOWN | __DTSEN;
                    else
                        ep0_i.STAT = __UOWN | __DTS | __DTSEN;
                } 
                else {
                    // Prepare for the Setup stage of a control transfer
                    prepare_for_setup_stage();
                }
            }
            TRNIF = 0;
        }

        if (IDLEIF) {
           SUSPND = 1;
           IDLEIF = 0;
       }
       USBIF = 0;
    }
}

void __interrupt(high_priority) genIntH(void) {
    _test02();
}

void USB_init() {

    UCFGbits.FSEN = 1;
    UCFGbits.UPUEN = 1;
    UCFGbits.UTRDIS = 0;
    UCFGbits.UOEMON = 0;
    UCFGbits.UTEYE = 0;
    UCFGbits.PPB = 0;
    
    UEP0bits.EP0HSHK = 1;
    UEP0bits.EP0CONDIS = 0;
    UEP0bits.EP0OUTEN = 1;
    UEP0bits.EP0INEN = 1;
    UEP0bits.EPSTALL = 0;
    
    
    UCONbits.SUSPND = 0;
    UCONbits.RESUME = 0;
    UCONbits.PKTDIS = 0;
    INTCONbits.TMR0IE = 0;
    INTCONbits.TMR0IF = 0;
    UIRbits.URSTIF = 0;
    UIRbits.IDLEIF = 0;
    UIRbits.SOFIF = 0;
    PIR2bits.USBIF = 0;
    PKTDIS = 0;
    BD0STAT.UOWN = 1;
    UCONbits.USBEN = 1;

    while(1);
}

void externTest() {
    table[1023] = 0;
    preferredConfig(0);
    USB_init();
}

static void preferredConfig(BYTE package) {
    
    BD0STAT.address = 0x500;
    BD0STAT.count = 64;
    BD0STAT.DTS = 0;
    BD0STAT.KEN = 0;
    BD0STAT.INCDIS = 0;
    BD0STAT.DTSEN = 1;
    BD0STAT.BSTALL = 0;
    BD0STAT.BC8 = 0;
    BD0STAT.BC9 = 0;
    BD0STAT.UOWN = 0;
    
//    BD1STAT.adrl = 0x40;
//    BD1STAT.adrh = 0x05;
    // BD1STAT.address = 0x500;
    // BD1STAT.count = 0;
    // BD1STAT.DTS = 1;
    // BD1STAT.KEN = 0;
    // BD1STAT.INCDIS = 0;
    // BD1STAT.DTSEN = 1;
    // BD1STAT.BSTALL = 0;
    // BD1STAT.BC8 = 0;
    // BD1STAT.BC9 = 0;
    // BD1STAT.UOWN = 0;
    
}
