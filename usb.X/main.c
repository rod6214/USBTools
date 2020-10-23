#include <xc.h>
#include <pic18f2550.h>
#include "i2c.h"

#define _XTAL_FREQ 4000000

// BYTE usb_buffer[RECEPTOR_LENGTH] __at(RECEPTOR_0_REG);
BYTE usb_buffer[3] __at(0x200);

void main () {

    PORTB = 0;
    PORTA = 0;
    PORTC = 0;
    TRISB = 3;
    TRISC = 0;
    TRISA = 255;
    ADCON1 = 15;

    PORTB |= 3;

    usb_buffer[0] = 0xAA;
    usb_buffer[1] = 0x77;
    // usb_buffer[0] = 7;

    I2C_t hI2c = {
        (int*)&PORTB,
        (int*)&TRISB
    };
    
    while (1)
    {
        int pressed = 0;

        while ((PORTA & 1) == 1) {
            if (pressed == 0) {
                start_serial(&hI2c);
                send_serial(&hI2c, 0x77, 2);
                wait_serial(&hI2c);
                stop_serial(&hI2c);
                pressed++;
            }
        }
    }
}
