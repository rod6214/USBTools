#define _XTAL_FREQ 48000000

#include "global.h"
#include "descriptors.h"

int dataIndex = 0;
int total_size = 24;

void main () 
{    
    PORTB = 0;
    TRISB = 0;
    USB_init();
    USB_WAIT();
    FLUSH(BD1_out);
    while(1);
}



