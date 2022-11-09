/*
 * gpio.h
 *
 *  Created on: Oct 31, 2022
 *      Author: Nelson
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_
#define GPIO_CONFIGURED 1
#define GPIO_DEFAULT	0
extern void GPIOC_set_pin13(void);
extern void GPIOC_clear_pin13(void);
extern void GPIOC_load_config(void);
extern int GPIOC_get_status(void);
#endif /* INC_GPIO_H_ */
