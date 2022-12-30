/*
 * PortController.h
 *
 *  Created on: Dec 27, 2022
 *      Author: Nelson
 */

#ifndef INC_PORTCONTROLLER_H_
#define INC_PORTCONTROLLER_H_

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
extern void Port_ProgramCPU(int value);
extern void Port_OnStep(enum ONE_STEP_MODE mode);
extern void Port_ResetCPU();

#endif /* INC_PORTCONTROLLER_H_ */
