#define LOGGER_ACTIVE
 #define _XTAL_FREQ 48000000
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cstream.h"
#include "prompt.h"
#include "constants.h"
#include "Tests/prompt_test.h"
#include "Tests/logger_test.h"

#define MAIN_BUFFER_LENGTH 64
/**
 * NOTE: 
 * You can activate the tests configuring the macro __TEST__ in the compiler
 */
char buffer[MAIN_BUFFER_LENGTH];
int index = 0;
int pending = TRUE;

int USB_SendData(STREAM pUsbStream, const char* data, int pkgIndex, int totalPackages, size_t bufferLength) 
{
	if (pkgIndex < totalPackages) 
	{
		memset(buffer, 0, bufferLength);
		size_t len = strlen(data);
		strncpy(buffer, data, len);
		WriteStream(pUsbStream, buffer, 0, len);
		pkgIndex++;
	}
	else 
	{
		memset(buffer, 0, bufferLength);
		WriteStream(pUsbStream, buffer, 0, bufferLength);
		pkgIndex = 0;
	}

	return pkgIndex;
}

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
					// Read usb memory and copy its content
                    memset(buffer, 0, MAIN_BUFFER_LENGTH);
					Stream_t* pUsbStream = usb_getStream();
					ReadStream(pUsbStream, buffer, 0);
                    usb_rewind();

                    // Decode commands
                   	commandLine(buffer, MAIN_BUFFER_LENGTH);
                   	char* command = getCommandKey();
                   	int isDev = strncmp(command, "dev", 8) == 0;
                    int isFin = strncmp(command, "fin", 8) == 0;
                    PORTB++;
					if (isDev) 
					{
						if (subCommandExists('v')) 
						{
							/**
							 * NOTE:
							 * We must send one package in an interrupt one at a time because HID requires steps.
							 * Don't disable interrupts or use FOR cicles individually.
							 */
							index = USB_SendData(pUsbStream, message_list[index], index, 5, MAIN_BUFFER_LENGTH);
						}
					}
                    else if (isFin) 
                    {
						index = USB_SendData(pUsbStream, "Hello world", index, 1, MAIN_BUFFER_LENGTH);
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
