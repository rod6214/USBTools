/*
 * core.c
 *
 *  Created on: Oct 31, 2022
 *      Author: Nelson
 */
#include "usb.h"

typedef struct
{
	uint16_t ADDR_TX;
	uint16_t RESERVED0;
	uint16_t COUNT_TX;
	uint16_t RESERVED1;
	uint16_t ADDR_RX;
	uint16_t RESERVED2;
	uint16_t COUNT_RX;
	uint16_t RESERVED3;
} BDx_t;

volatile BDx_t BD0 __attribute__((section(".ep_bd")));
volatile BDx_t BD1 __attribute__((section(".ep_bd")));
volatile uint32_t EP0_in_buffer[8] __attribute__((section(".ep_ram")));
volatile uint32_t EP0_out_buffer[8] __attribute__ ((section(".ep_ram")));
volatile uint32_t EP1_in_buffer[64] __attribute__((section(".ep_ram")));
volatile uint32_t EP1_out_buffer[64] __attribute__ ((section(".ep_ram")));

#define BUFFER_OFFSET(x) (((uint32_t)x&0x3ff)>>1)

void USB_init()
{
	USB->CNTR |= USB_CNTR_RESETM;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	USB->DADDR = USB_DADDR_EF;
	USB->EP0R = USB_EP_CONTROL | USB_EP_TYPE_MASK;
	BD0.ADDR_TX = 64;
	BD0.COUNT_TX = 8U;
	BD0.ADDR_RX = 128;
	BD0.COUNT_RX = 0xBC00;
//	USB->BTABLE = BTABLE_ADDRESS;
//	USB->CNTR &= ~USB_CNTR_FRES;
	USB->ISTR = 0U;
//
//	HAL_Delay(5);
}

void USB_process_reset()
{
//	USB->DADDR = USB_DADDR_EF;
//	USB->EP0R = USB_EP_CONTROL | USB_EP_TYPE_MASK;
//	BD0.ADDR_TX = 128;
//	BD0.COUNT_TX = 8U;
//	BD0.ADDR_RX = 64;
//	BD0.COUNT_RX = 0xBC00;
//	USB->CNTR |= USB_CNTR_CTRM;
//	USB->EP0R |= (3 << USB_EP0R_STAT_RX_Pos);
//	USB->EP0R |= (3 << USB_EP0R_STAT_TX_Pos);
//	USB->CNTR &= ~USB_CNTR_FRES;
}

void USB_create_buffers()
{

}
