/*
 * PortController.h
 *
 *  Created on: Dec 27, 2022
 *      Author: Nelson
 */

#ifndef INC_PORTCONTROLLER_H_
#define INC_PORTCONTROLLER_H_

#define SET_AS_INPUT(x, y) { GPIO_InitTypeDef GPIO_InitStruct = {0}; \
	GPIO_InitStruct.Pin = x; \
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
	GPIO_InitStruct.Pull = GPIO_NOPULL; \
	HAL_GPIO_Init(y, &GPIO_InitStruct); }

#define SET_AS_OUTPUT(x, y) {GPIO_InitTypeDef GPIO_InitStruct = {0}; \
	GPIO_InitStruct.Pin = x; \
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
	GPIO_InitStruct.Pull = GPIO_NOPULL; \
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
	HAL_GPIO_Init(y, &GPIO_InitStruct);}

enum ONE_STEP_MODE
{
	ACTIVATED,
	DEACTIVATED,
	CLOCK_OFF
};

extern void Port_Write(char* buffer, int offset, int bytes);
extern void Port_Read(char* buffer, int offset, int bytes);
extern void Port_init();
extern void Port_Reset();
extern void Port_Set();
extern void Port_AsInput();
extern void Port_AsOutput();
extern void Port_ProgramCPU();
extern void Port_OneStep();
extern void Port_ResetCPU();
extern void Port_RunCPU();
extern void Port_OneStepClock();
extern void Port_Status(char* buffer);
extern void Port_DeactivateControl();
extern void Port_ActivateControl();

#endif /* INC_PORTCONTROLLER_H_ */
