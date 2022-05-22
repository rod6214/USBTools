/*
 * File:   Int_Handle.c
 * Author: Nelson
 *
 * Created on April 24, 2022, 12:40 PM
 */

#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cstream.h"
#include "prompt.h"

#define MAIN_BUFFER_LENGTH 64
char buffer[MAIN_BUFFER_LENGTH];
int index = 0;
Stream_t* pUsbStream;

char* getCommand() {
    memset(buffer, 0, MAIN_BUFFER_LENGTH);
    pUsbStream = usb_getStream();
	ReadStream(pUsbStream, buffer, 0);
    usb_rewind();
    // Decode commands
    commandLine(buffer, MAIN_BUFFER_LENGTH);
    char* command = getCommandKey();
    return command;
}

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
	}

	return pkgIndex;
}

char* readMemory()
{
    PORTB = 0;
    TRISB = 255;
    
}

void __interrupt(high_priority) high_isr(void) {
    struct USBHandler* usbPtr;
    
    if(PIR2bits.USBIF)
	{
        usbPtr = usb_handler();
		PIR2bits.USBIF=0;
        
        switch (usbPtr->Status)
		{
			case DATA_RECEIVED:
			{
                char* pCommand = getCommand();
                int isRead = strncmp(pCommand, "read", 8) == 0;
//                int isWrite = strncmp(command, "write", 8) == 0;
                int isEmptyToken = strncmp(pCommand, "", 8) == 0;
                int isEnd = strncmp(pCommand, "-", 8) == 0;
                
                if (isRead) 
                {
                    if (subCommandExists('d'))
                    {
//                        char* pValue = getSubCommandValue('d');
                        
                        // TODO: Send the data after read from rom
                        index = USB_SendData(pUsbStream, "SET HERE DATA FOR PC", index, 1, MAIN_BUFFER_LENGTH);
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
//                    PORTB++;
                    /**
                     * NOTE:
                     * If the command is empty and index is 0,
                     * we need to inform that the command is incorrect
                     */
                    index = USB_SendData(pUsbStream, "Command not found.", index, 1, MAIN_BUFFER_LENGTH);
                }
            }
            break;
        }
    }
}
