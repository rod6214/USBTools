#if __USB__
#ifndef USB_H
#define USB_H

#include <xc.h>
#include "pic18f2550.h"

extern unsigned char usb_device_state;

#define USB_BUFFER_LEN 64
#define USB_SELF_POWERED 1
#define USB_MAXPOWER 100

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

#define DATA_RECEIVED 0x35
#define HID_RPT01_SIZE 29

#include "usbpic_defs.h"

// initialize usbcdc module
extern void usb_init(void);
// handle usb control messages, poll atleast every 1ms or call from IRQ
extern void* usb_handler(void);
extern int usb_putchar(char c);
extern char usb_getchar();
extern void usb_write(BYTE* data, unsigned int length);
extern void rewind();
extern void* usb_getStream();

struct USBHandler {
    int Status;
    unsigned int Length;
    void* pTxBuffer;
    void* pRxBuffer;
};

#endif
#endif
