#include <xc.h>
#include <pic18f2550.h>
#include "i2c.h"

#define _XTAL_FREQ 4000000

// BYTE usb_buffer[RECEPTOR_LENGTH] __at(RECEPTOR_0_REG);
BYTE usb_buffer[5] __at(0x200);

void main () {

    PORTB = 3;
    PORTA = 0;
    PORTC = 0;
    TRISB = 0;
    TRISC = 0;
    TRISA = 255;
    ADCON1 = 15;

    usb_buffer[0] = 0xA0;
    usb_buffer[1] = 0x00;
    usb_buffer[2] = 0x00;
    usb_buffer[3] = 0x99;
    usb_buffer[4] = 0x99;

    I2C_t hI2c = {
        (int*)&PORTB,
        (int*)&TRISB
    };
    
    while (1)
    {
        int pressed = 0;

        while ((PORTA & 1) == 1) {
            if (pressed == 0) {
                receive_serial(&hI2c, usb_buffer, 1);
                // send_serial(&hI2c, usb_buffer, 4);
                // __delay_ms(1);
                // usb_buffer[0] = 0xA1;
                // receive_serial(&hI2c, usb_buffer, 2);
                // send_serial(&hI2c, usb_buffer, 1);

                pressed++;
            }
        }
    }
}
