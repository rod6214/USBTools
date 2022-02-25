#define LOGGER_ACTIVE
#define __STREAM__
// #define _XTAL_FREQ 4000000
//#include "pic18f2550.h"
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>
#include "logger.h"
//#include "kernel.h"



void __interrupt(high_priority) high_isr(void)
{
//	struct USBHandler* usbPtr;
//	const char* data = "Hello world!!!";
//    int idx = 0;
//    int finish = 0;
//    
//	if(PIR2bits.USBIF)
//	{
//		usbPtr = usb_handler();
//		PIR2bits.USBIF=0;
//
//		switch (usbPtr->Status)
//		{
//			case DATA_RECEIVED:
//				{
//					PORTB = usb_getchar();
//                    
//                    while(!finish) 
//                    {
//                        char c = data[idx];
//                        if (idx < 14) {
//							c = data[idx];
//						}
//						else {
//							c = 0;
//						}
//                        finish = usb_putchar(c);
//                        idx++;
//                    }
//				}
//				break;
//			
//			default:
//				break;
//		}
//	}
}

void __interrupt(low_priority) low_isr(void)
{
	// ;
}

// struct VALUES {
//     int x;
//     long y;
// };

//void test_ptr(void* ptr) 
//{
//    PORTB = (char)ptr;
//}

void main(void) {
    
	while (1) {}

}
