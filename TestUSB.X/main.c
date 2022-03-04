#define LOGGER_ACTIVE
// #define _XTAL_FREQ 4000000
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include "cstream.h"
#include "prompt.h"
#include "Tests/prompt_test.h"

/**
 * Note: You can activate the tests configuring the macro __TEST__ in the compiler
 * 
 */

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
                    int isDev = strncmp(command, "dev", 8) == 0;
                    
                    if (isDev) 
                    {
						if (subCommandExists('v')) 
						{
							const char* pMsg = message_list[0];
							ProgramMemToStream(pMsg, 0, 0, 52);
						}
                    }
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
    TRISB = 0;
    int fails = executeTests();
    PORTB = fails;
	while (1) {}

}
