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
#include "pwm.h"

#define MAIN_BUFFER_LENGTH 64
/**
 * NOTE: 
 * You can activate the tests configuring the macro __TEST__ in the compiler
 */
char buffer[MAIN_BUFFER_LENGTH];
int index = 0;
int pending = TRUE;
PWM_t pwmData;

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
//		pkgIndex = 0;
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
                    int isPwm = strncmp(command, "pwm", 8) == 0;
                    int isEnd = strncmp(command, "-", 8) == 0;
                    int isEmptyToken = strncmp(command, "", 8) == 0;
                    
					if (isDev && subCommandExists('v')) 
					{
						/**
						 * NOTE:
						 * We must send one package in an interrupt one at a time because HID requires steps.
						 * Don't disable interrupts or use FOR cicles individually.
						 */
						index = USB_SendData(pUsbStream, message_list[index], index, 2, MAIN_BUFFER_LENGTH);
					}
                    else if (isFin) 
                    {
						index = USB_SendData(pUsbStream, "Hello world", index, 1, MAIN_BUFFER_LENGTH);
                    }
                    else if (isPwm) 
                    {
                        if (subCommandExists('i')) 
                        {
                            pwm_setFreq(&pwmData, PWM_FREQ_50K);
                            pwm_setCCPPort(&pwmData, PWM_RB3);
                            pwm_turnOnCPPModules(&pwmData, PWM_CPP2);
                            pwm_init(&pwmData);
                            pwm_setPulse(&pwmData, 2);
                            index = USB_SendData(pUsbStream, "PWM started :)", index, 1, MAIN_BUFFER_LENGTH);
                        }
                        else if (subCommandExists('d'))
                        {
                            char* pValue = getSubCommandValue('d');
                            int value = atoi(pValue);
                            pwm_setPulse(&pwmData, value + 1);
                            index = USB_SendData(pUsbStream, "PWM received :)", index, 1, MAIN_BUFFER_LENGTH);
                        }
                        else if (subCommandExists('o')) 
                        {
                            pwm_finish(&pwmData);
                            index = USB_SendData(pUsbStream, "PWM finished :(", index, 1, MAIN_BUFFER_LENGTH);
                        }
                        else 
                        {
                            index = USB_SendData(pUsbStream, "PWM error: SubCommand not found.", index, 1, MAIN_BUFFER_LENGTH);
                        }
                    }
                    else if (isEmptyToken) 
                    {
                        index = 0;
                    }
                    else if (!isEnd)
                    {
                        PORTB++;
                        /**
                         * NOTE:
                         * If the command is empty and index is 0,
                         * we need to inform that the command is incorrect
                         */
                        index = USB_SendData(pUsbStream, "Command not found.", index, 1, MAIN_BUFFER_LENGTH);
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
//    TRISB = 0;
//    PORTB = 0;
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
