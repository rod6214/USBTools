/*
 * usb.h
 *
 *  Created on: Oct 31, 2022
 *      Author: Nelson
 */

#ifndef INC_USB_H_
#define INC_USB_H_
#include "stm32f1xx_hal.h"



#define IS_RESET (USB->ISTR & USB_ISTR_RESET)
#define IS_CTR (USB->ISTR & USB_ISTR_CTR)

extern void USB_init();
extern void USB_create_buffers();
extern void USB_process_reset();

#endif /* INC_USB_H_ */
