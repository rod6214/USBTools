/* 
File: usbcdc.h

 Copyright (c) 2010,2013 Kustaa Nyholm / SpareTimeLabs

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 
Version 1.1     Compatible with SDCC 3.x 
                Respond to GET_LINE_CODING to work with overzealous 
                Windows software (like Hyperterminal)
*/

#ifndef USBCDC_H
#define USBCDC_H

extern unsigned char usbcdc_device_state;

#define USBCDC_BUFFER_LEN 32
#define USBCDC_SELF_POWERED 1
#define USBCDC_MAXPOWER 100

#include "usbcdc_defs.h"
#include "usbpic_defs.h"

// initialize usbcdc module
void usbcdc_init(void);

// waitiuntil the device is configure (if you care)
void cdc_wait_config();


// handle usb control messages, poll atleast every 1ms or call from IRQ
void usbcdc_handler(void);

void usbcdc_putchar(char c);

void usbcdc_flush();

char usbcdc_getchar() ;

//#pragma udata usbram5 setup_packet control_transfer_buffer cdc_rx_buffer cdc_tx_buffer cdcint_buffer
#define RAM_BUFFER_BASE 0x500
#define SETUP_PACKET_REG RAM_BUFFER_BASE
#define CONTROL_TRANSFER_REG (SETUP_PACKET_REG + E0SZ)
#define CDC_TX_REG (CONTROL_TRANSFER_REG + E0SZ)
#define CDC_RX_REG (CDC_TX_REG + USBCDC_BUFFER_LEN)
#define CDC_INT_REG (CDC_RX_REG + USBCDC_BUFFER_LEN)
#define HID_RPT01_SIZE 28

volatile setup_packet_struct setup_packet __at(SETUP_PACKET_REG);
extern unsigned char control_transfer_buffer[E0SZ] __at(CONTROL_TRANSFER_REG);
extern volatile unsigned char cdc_tx_buffer[USBCDC_BUFFER_LEN] __at(CDC_TX_REG);
extern volatile unsigned char cdc_rx_buffer[USBCDC_BUFFER_LEN] __at(CDC_RX_REG);
extern volatile unsigned char cdcint_buffer[USBCDC_BUFFER_LEN] __at(CDC_INT_REG);

char usbcdc_wr_busy();
void usbcdc_write(unsigned char len);
extern void usb_write(BYTE* data, int length);
unsigned char usbcdc_rd_ready();
void usbcdc_read();
#define usbcdc_rd_len() (ep2_o.CNT)

#endif
