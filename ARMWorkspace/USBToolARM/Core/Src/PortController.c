/*
 * PortController.c
 *
 *  Created on: Dec 27, 2022
 *      Author: Nelson
 */

#include "PortController.h"
#include "timers.h"

#define SHIFT_CLOCK GPIO_PIN_0
#define PORT_RESET GPIO_PIN_9
#define LATCH_CLOCK GPIO_PIN_7
#define OUTPUT_ENABLE GPIO_PIN_8
#define OUTPUT_DATA_H GPIO_PIN_5
#define OUTPUT_DATA_L GPIO_PIN_6
#define READY_SIGNAL GPIO_PIN_13
#define RESET_CPU GPIO_PIN_12
#define PROGRAM_CPU GPIO_PIN_13
#define ONE_STEP0 GPIO_PIN_14
#define ONE_STEP1 GPIO_PIN_15

static void set_address(int value);

void Port_ResetCPU()
{
	int k = 0;

	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_SET);

	while (k < 2)
	{
		HAL_GPIO_TogglePin(GPIOB, RESET_CPU);
		delay_us(1);
		k++;
	}
}

void Port_ProgramCPU(int value)
{
	if (value)
	{
		HAL_GPIO_WritePin(GPIOB, PROGRAM_CPU, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, PROGRAM_CPU, GPIO_PIN_RESET);
	}
}

void Port_OnStep(enum ONE_STEP_MODE mode)
{
	switch(mode)
	{
	case ACTIVATED:
		break;
	case DEACTIVATED:
		break;
	case CLOCK_OFF:
		break;
	}
}

void Port_Write(char* buffer, int offset, int bytes)
{
	Port_Set();
	Port_AsOutput();
	for (int i = offset; i < (offset + bytes); i++)
	{
		set_address(i);
		GPIOA->ODR = buffer[i - offset];
	}
}

void Port_init()
{
	HAL_GPIO_WritePin(GPIOC, READY_SIGNAL, GPIO_PIN_SET);
	HAL_Delay(1);
	Port_Reset();
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, READY_SIGNAL, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void Port_Read(char* buffer, int offset, int bytes)
{
	Port_Set();
	Port_AsInput();
	for (int i = offset; i < (offset + bytes); i++)
	{
		set_address(i);
		buffer[i - offset] = (0xff) & (GPIOA->IDR);
	}
}

void Port_Set()
{
	HAL_GPIO_WritePin(GPIOB, OUTPUT_ENABLE, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, PORT_RESET, GPIO_PIN_SET);
}

void Port_Reset()
{
	HAL_GPIO_WritePin(GPIOB, OUTPUT_ENABLE, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SHIFT_CLOCK | LATCH_CLOCK | PORT_RESET
			| OUTPUT_DATA_H | OUTPUT_DATA_L, GPIO_PIN_RESET);
}

void Port_AsOutput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
								|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Port_AsInput()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
	                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void set_address(int value)
{
	char addressH = ((0xff00 & value) >> 8);
	char addressL = (0xff & value);

	for (int i = 0; i < 8; i++)
	{
		if ((128 & addressH) == 128)
		{
			HAL_GPIO_WritePin(GPIOB, OUTPUT_DATA_H, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, OUTPUT_DATA_H, GPIO_PIN_RESET);
		}

		if ((128 & addressL) == 128)
		{
			HAL_GPIO_WritePin(GPIOB, OUTPUT_DATA_L, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, OUTPUT_DATA_L, GPIO_PIN_RESET);
		}

		int j = 0;

		while (j < 2)
		{
			HAL_GPIO_TogglePin(GPIOB, SHIFT_CLOCK);
			delay_us(1);
			j++;
		}

		addressH <<= 1;
		addressL <<= 1;
	}

	int k = 0;

	while (k < 2)
	{
		HAL_GPIO_TogglePin(GPIOB, LATCH_CLOCK);
		delay_us(1);
		k++;
	}
}


