#ifndef USB_H
#define USB_H

#include <xc.h>
#include "pic18f2550.h"

extern unsigned char usb_device_state;

#define USB_BUFFER_LEN 32
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

#include "usbpic_defs.h"

// initialize usbcdc module
void usb_init(void);
// handle usb control messages, poll atleast every 1ms or call from IRQ
void usb_handler(void);
void usb_putchar(char c);
char usb_getchar();
void usb_write(BYTE* data, unsigned int length);

#define RAM_BUFFER_BASE 0x500
#define SETUP_PACKET_REG RAM_BUFFER_BASE
#define CONTROL_TRANSFER_REG (SETUP_PACKET_REG + ENDPOINT_0_SIZE)
#define TX_REG (CONTROL_TRANSFER_REG + ENDPOINT_0_SIZE)
#define RX_REG (TX_REG + USB_BUFFER_LEN)
#define INT_REG (RX_REG + USB_BUFFER_LEN)
#define HID_RPT01_SIZE 28

volatile setup_packet_struct setup_packet __at(SETUP_PACKET_REG);
volatile unsigned char control_transfer_buffer[ENDPOINT_0_SIZE] __at(CONTROL_TRANSFER_REG);
volatile unsigned char tx_buffer[USB_BUFFER_LEN] __at(TX_REG);
volatile unsigned char rx_buffer[USB_BUFFER_LEN] __at(RX_REG);
volatile unsigned char  cdcint_buffer[USB_BUFFER_LEN] __at(INT_REG);

#endif
