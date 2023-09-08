#include "descriptor.h"
#include "usbtypes.h"

void __interrupt(high_priority) high_isr(void)
{
	if(PIR2bits.USBIF)
	{
		usb_interrupt_handler();
		PIR2bits.USBIF=0;
	}
}

BYTE buffer[64];
BYTE buffer_w[64];

void main () {
    PORTB = 0;
    TRISB = 0;
    TRISA = 0;

    set_descriptors((codePtr) &device_descriptor, 
    (codePtr) &config_descriptor, (codePtr) &hid_rpt01, (codePtr*) string_descriptors);

    usb_init();

    UIEbits.TRNIE = 1;
    INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;
    buffer_w[63] = (BYTE)'H';

    while (1)
    {
        usb_read(EP1, buffer, 64);
        PORTB = buffer[63];
        usb_write(EP1, buffer_w, 64);
        /* code */
    }
}
