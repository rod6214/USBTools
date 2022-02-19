// #define _XTAL_FREQ 4000000
#include "pic18f2550.h"
#include "usb.h"
#include "usb_defs.h"
#include <xc.h>


void __interrupt(high_priority) high_isr(void)
{
	if(PIR2bits.USBIF)
	{
		usb_handler();
		PIR2bits.USBIF=0;
	}
}

void __interrupt(low_priority) low_isr(void)
{
	;
}


// void putchar(char c)
// {
// 	if (c=='\n') {
// 		usbcdc_putchar('\r');
// 		}

// 	usbcdc_putchar(c);
// 	if (c=='\n')
// 		usbcdc_flush();
// }

// char getchar() {
// 	usbcdc_flush();
// 	return usbcdc_getchar();
// }

const char params[] = {
3,4,5
};

typedef void (*my)(const char* params);

//void test(my call) {
//    call(params);
//}
//
//void testCallback(const char* params) {
//    PORTB = 3;
//}



void main(void) {
//	 OSCCON = 0x70;
//    UCFG = UPUEN;
	PORTB = 0;
	PORTC = 0;
	TRISB = 0;
	TRISC = 1;
    // my pepe = testCallback;
    
    // pepe(params);
//    test(params);
//	    di();
//    IPR2bits.USBIP = 1;
//    INTCONbits.PEIE = 0;
//    PIE2bits.USBIE = 1;
    // Reset USB interrupts    
//    UIR = 0;
//    UIEbits.URSTIE = 0;// occur 2 times
//    UIEbits.SOFIE = 0;// many times we couldn't count it
    // UIEbits.TRNIE = 1;// occur 1 time
//    UIEbits.ACTVIE = 1;// occur 1 time
//    UIEbits.IDLEIE = 1;// occur 2 times
//    UIEbits.URSTIE = 1;// occur 4 times
//    STALLIE = 0; 
//    UERRIE = 0;
//    RCONbits.IPEN = 1;
    // SUSPND = 0;
//    INTCONbits.GIEH = 1;

	usb_init();
//
	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
//	// INTCONbits.GIEH = 1;
//
	while (usb_device_state != CONFIGURED)
		;
//	while (PORTC == 0);
	// PORTB++;
//
//	BYTE tmp[4];
//
//	tmp[0] = 'P';
//	tmp[1] = 'O';
//	tmp[2] = 'I';
//
//	usb_write(tmp, 2);
	// printft("Wellcome!\n");

	while (1) {
		// printft("%02d\n",getchar());
	}

}
