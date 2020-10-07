/* 
This file is licensed under the MIT license:

Copyright (c) 2010,2013 Kustaa Nyholm / SpareTimeLabs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

 Version 1.1     Compatible with SDCC 3.x

 */


#include "pic18f2550.h"
#include "usbcdc.h"
#include "usb_defs.h"
#include "pic-config.c"
#include "printft.h"
#include <xc.h>


void __interrupt(high_priority) high_isr(void)
{
	if(PIR2bits.USBIF)
	{
		usbcdc_handler();
		PIR2bits.USBIF=0;
	}
}

void __interrupt(low_priority) low_isr(void)
{
	;
}


void putchar(char c)
{
	if (c=='\n') {
		usbcdc_putchar('\r');
		}

	usbcdc_putchar(c);
	if (c=='\n')
		usbcdc_flush();
}

char getchar() {
	usbcdc_flush();
	return usbcdc_getchar();
}

void main(void) {
//	 OSCCON = 0x70;
//    UCFG = UPUEN;
	PORTB = 0;
	PORTC = 0;
	TRISB = 0;
	TRISC = 1;

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

	usbcdc_init();

	INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
	// INTCONbits.GIEH = 1;

	while (usbcdc_device_state != CONFIGURED)
		;
	while (PORTC == 0);
	PORTB++;

	BYTE tmp[4];

	tmp[0] = 'P';
	tmp[1] = 'O';
	tmp[2] = 'I';

	usb_write(tmp, 2);
	// printft("Wellcome!\n");

	while (1) {
		// printft("%02d\n",getchar());
	}

}
