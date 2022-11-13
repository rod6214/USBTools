#include <xc.h>
#include <pic18f2550.h>
#include "descriptors.h"
#include "usb.h"
#include "usbtypes.h"

 #define _XTAL_FREQ 4000000

void __interrupt(high_priority) high_isr(void)
{
	if(PIR2bits.USBIF)
	{
		usb_interrupt_handler();
		PIR2bits.USBIF=0;
	}
}

BYTE usb_buffer[RECEPTOR_LENGTH] __at(RECEPTOR_0_REG);

void main () {

    PORTB = 0;
    PORTA = 0;
    TRISB = 0;
    TRISA = 255;
    ADCON1 = 15;

    set_descriptors((codePtr) &device_descriptor, 
    (codePtr) &config_descriptor, NULL, (codePtr*) string_descriptors);

    usb_init();

    UIEbits.TRNIE = 1;
    INTCONbits.PEIE = 1;
	INTCONbits.GIE = 1;

    while(get_device_state() != CONFIGURED);
    
    while (1)
    {
        usb_read(EP1, usb_buffer, 8);
        PORTB = usb_buffer[0];
        // int pressed = 0;

        // while ((PORTA & 1) == 1) {
        //     if (pressed == 0) {
        //         usb_buffer[0] = 10;
        //         usb_buffer[1] = 10;
        //         usb_buffer[2] = 10;
        //         usb_write(EP1, usb_buffer, 3);
        //         usb_buffer[0] = 1;
        //         usb_buffer[1] = 2;
        //         usb_buffer[2] = 3;
        //         usb_write(EP1, usb_buffer, 3);
        //         // usb_read(EP1, usb_buffer, 3);
        //         // usb_write(usb_buffer);
        //         //  PORTB++;
        //         pressed++;
        //     }
        // }
//        while(usb_read(usb_buffer) == 0);
//        
//        PORTB = usb_buffer[0];
//        
//        usb_buffer[0]++;
//        
//        __delay_ms(500);
//        
//        usb_write(usb_buffer);
//
//        PORTB++;
    }
}
