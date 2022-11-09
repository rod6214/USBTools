/*
 * gpio.c
 *
 *  Created on: Oct 31, 2022
 *      Author: Nelson
 */
#include "stm32f1xx_hal.h"
#include "gpio.h"


static int __gpio_status = GPIO_DEFAULT;

void GPIOC_set_pin13()
{
	GPIOC->BSRR |= GPIO_BSRR_BR13;
}

void GPIOC_clear_pin13()
{
	GPIOC->BSRR = GPIO_BSRR_BS13;
}

void GPIOC_load_config()
{
	GPIOC->CRH = 0;
	GPIOC->CRH = (3 << GPIO_CRH_MODE13_Pos);
	GPIOC->BSRR = GPIO_BSRR_BS13;
	__gpio_status = GPIO_CONFIGURED;
}

int GPIOC_get_status()
{
	return __gpio_status;
}
