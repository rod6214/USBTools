#define LOGGER_ACTIVE
// #define _XTAL_FREQ 4000000
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include <string.h>
#include "cstream.h"
#include "prompt.h"
#include "constants.h"
#include "Tests/prompt_test.h"
#include "Tests/logger_test.h"


/**
 * Note: You can activate the tests configuring the macro __TEST__ in the compiler
 * 
 */
char buffer[64];

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
                    memset(buffer, 0, 64);
					Stream_t* pUsbStream = usb_getStream();
					ReadStream(pUsbStream, buffer, 0);
                    usb_rewind();

                   	commandLine(buffer, 64);
                   	char* command = getCommandKey();
                   	int isDev = strncmp(command, "dev", 8) == 0;

					 if (isDev) 
					 {
					 	 if (subCommandExists('v')) 
					 	 {
					 	 	PORTB = 1;
					 	 }
					 	 else if (subCommandExists('d')) 
					 	 {
					 	 	PORTB = 2;
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
    PORTB = 0;
    usb_SetUsbAsHighPriority();
    usb_init();
    #if __TEST_PROMPT__
    TRISB = 0;
    int prompt_fails = Prompt_ExecuteTests();
    PORTB = fails;
    #endif
    #if __LOGGER_TEST__
    TRISB = 0;
    int logger_fails = Logger_ExecuteTests();
    PORTB = (char)logger_fails;
    #endif
	while (1) {}

}
