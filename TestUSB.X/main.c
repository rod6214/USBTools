#define LOGGER_ACTIVE
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
	;
}

struct VALUES {
    int x;
    long y;
};

//void test_ptr(void* ptr) 
//{
//    PORTB = (char)ptr;
//}

void main(void) {
    CreateLogger(7);
//	TRISB = 0;
//	List_t* l1 = CreateList(12);
//    List_t* l2 = CreateList(4);
//    List_t* l3 = CreateList(6);
//
//	kpush(l1, 'a');
//    kpush(l1, 'b');
//	kpush(l1, 'c');
//	kpush(l1, 'd');
//	kpush(l1, 'e');
//	PORTB = kpop(l1);
//    
//    kpush(l2, 'a');
//    kpush(l2, 'b');
//	kpush(l2, 'c');
//	kpush(l2, 'd');
//    PORTB = kpop(l2);
//    
//    kpush(l3, 'a');
//    kpush(l3, 'b');
//	kpush(l3, 'c');
//	kpush(l3, 'd');
//    kpush(l3, 'e');
//	kpush(l3, 'f');
//    kfree(l2);
//    
//    List_t* l4 = CreateList(8);
//    kpush(l4, '1');
//    kpush(l4, '2');
//	kpush(l4, '3');
//	kpush(l4, '4');
//    kpush(l4, '5');
//	kpush(l4, '6');
//    kpush(l4, '7');
//    PORTB = kpop(l4);
    // TRISB = 0;
    // kmalloc(2);
    // void* pMem = kmalloc(10);
    // kmalloc(15);
    // kfree(pMem);
    // kmalloc(12);
//    List_t* list = CreateList();
//    kpush(list, 'a');
//    kpush(list, 'b');
//    kpush(list, 'c');
//    int count = kcount(list);
//    TRISB = 0;
//    
//    if (count > 0)
//    PORTB = (char)count;
//    
//    void* begin = (void*)list;
//    void* test = keval(begin);
//    char c;
    
//    {
//        foreach(l4) 
//        {
//            char c = kgetchar(l4);
//            PORTB = c;
//        }
//    }
    
    
    
//    for (char c = kgetchar(begin); test; kinc(begin)) 
//    {
//        PORTB = c;
//    }
    
	while (1) {}

}
