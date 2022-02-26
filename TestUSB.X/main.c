#define LOGGER_ACTIVE
//#define __STREAM__
// #define _XTAL_FREQ 4000000
//#include "pic18f2550.h"
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
//#include "logger.h"
//#include "printft.h"
//#include "kernel.h"

#include "prompt.h"

void __interrupt(high_priority) high_isr(void)
{
	struct USBHandler* usbPtr;
    
	if(PIR2bits.USBIF)
	{
		usbPtr = usb_handler();
		PIR2bits.USBIF=0;

		switch (usbPtr->Status)
		{
			case DATA_RECEIVED:
				{
                    commandLine((char*)rx_buffer);
                    char* command = getCommandKey();
                    int cmd = strncmp(command, "version", 8);
                    
                    if (cmd == 0) 
                    {}
//                    getSubCommandValue('v');
				}
				break;
			
			default:
				break;
		}
	}
}

void __interrupt(low_priority) low_isr(void)
{
	// ;
}

void main(void) 
{
	while (1) {}

}
