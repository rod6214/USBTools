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


/**
 * Note: You can activate the tests configuring the macro __TEST__ in the compiler
 * 
 */
char buffer[32];

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
                    char* rx_buffer = (char*)(usbPtr->pRxBuffer);
                    commandLine(rx_buffer, 64);
                    char* command = getCommandKey();
                    int isDev = strncmp(command, "dev", 8) == 0;
                    
                    if (isDev) 
                    {
						if (subCommandExists('v')) 
						{
//                            size_t len = strlen(message_list[0]);
//                            strncpy(buffer, message_list[0], len);
//							const char* pMsg = message_list[0];
//							ProgramMemToStream(pMsg, 0, 0, 52);
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
    #if __TEST_PROMPT__
    TRISB = 0;
    int fails = executeTests();
    PORTB = fails;
    #endif
	while (1) {}

}
