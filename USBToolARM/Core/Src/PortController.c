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
#define WRITE_ENABLE GPIO_PIN_11
#define RESET_CPU GPIO_PIN_12
//#define PROGRAM_CPU GPIO_PIN_13
#define BUS_REQUEST GPIO_PIN_13
#define ONE_STEP GPIO_PIN_14
#define ONE_STEP_CLOCK GPIO_PIN_9
#define BUS_CPU_ACK GPIO_PIN_10
#define RUN_CPU GPIO_PIN_15

static void set_address(int value);
static void write_enable();
static void write_long_enable();

void Port_ResetCPU()
{
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_RESET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_SET);
	delay_us(1);
}

void Port_ProgramCPU()
{
	HAL_GPIO_WritePin(GPIOB, BUS_REQUEST, GPIO_PIN_RESET);
	// Wait until ack is received
	while(HAL_GPIO_ReadPin(GPIOA, BUS_CPU_ACK));
	Port_ActivateControl();
}

void Port_Status(char* buffer)
{
	buffer[0] = 0xff & HAL_GPIO_ReadPin(GPIOB, BUS_REQUEST);
	buffer[1] = 0xff & HAL_GPIO_ReadPin(GPIOB, ONE_STEP);
	buffer[2] = 0xff & HAL_GPIO_ReadPin(GPIOB, RUN_CPU);
}

void Port_RunCPU()
{
	Port_Reset();
	HAL_GPIO_WritePin(GPIOB, BUS_REQUEST, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, ONE_STEP, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RUN_CPU, GPIO_PIN_SET);
}

void Port_OneStep()
{
	Port_Reset();
	HAL_GPIO_WritePin(GPIOB, BUS_REQUEST, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, RUN_CPU, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, RESET_CPU, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, ONE_STEP, GPIO_PIN_SET);
}

void Port_OneStepClock()
{
	HAL_GPIO_WritePin(GPIOA, ONE_STEP_CLOCK, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOA, ONE_STEP_CLOCK, GPIO_PIN_RESET);
	delay_us(1);
}

void Port_Write(char* buffer, int offset, int bytes)
{
	Port_Set();
	Port_AsOutput();
	HAL_GPIO_WritePin(GPIOA, OUTPUT_ENABLE, GPIO_PIN_SET);
	for (int i = offset; i < (offset + bytes); i++)
	{
		set_address(i);
		int temp = 0xffffff00 & (GPIOA->ODR);
		GPIOA->ODR = temp | buffer[i - offset];
		if ((i - offset) < 63)
			write_enable();
		else
			write_long_enable();
	}
}

void Port_init()
{
	HAL_GPIO_WritePin(GPIOA, ONE_STEP_CLOCK, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, READY_SIGNAL, GPIO_PIN_SET);
	HAL_Delay(1);
	Port_Reset();
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, READY_SIGNAL, GPIO_PIN_RESET);
	HAL_Delay(1);
	Port_Reset();
}

void Port_Read(char* buffer, int offset, int bytes)
{
	Port_Set();
	Port_AsInput();
	HAL_GPIO_WritePin(GPIOA, OUTPUT_ENABLE, GPIO_PIN_RESET);
	for (int i = offset; i < (offset + bytes); i++)
	{
		set_address(i);
		buffer[i - offset] = (0xff) & (GPIOA->IDR);
	}
	HAL_GPIO_WritePin(GPIOA, OUTPUT_ENABLE, GPIO_PIN_SET);
}

void Port_Set()
{
	HAL_GPIO_WritePin(GPIOB, OUTPUT_ENABLE, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, PORT_RESET, GPIO_PIN_SET);
}

void Port_Reset()
{
	Port_AsInput();
	HAL_GPIO_WritePin(GPIOB, RUN_CPU, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, OUTPUT_ENABLE, GPIO_PIN_SET);
	Port_DeactivateControl();
	HAL_GPIO_WritePin(GPIOB, SHIFT_CLOCK | LATCH_CLOCK | PORT_RESET
			| OUTPUT_DATA_H | OUTPUT_DATA_L, GPIO_PIN_RESET);
}

void Port_DeactivateControl()
{
//	SET_AS_INPUT(WRITE_ENABLE, GPIOB);
	SET_AS_INPUT(OUTPUT_ENABLE, GPIOA);
}

void Port_ActivateControl()
{
//	SET_AS_OUTPUT(WRITE_ENABLE, GPIOB);
	SET_AS_OUTPUT(OUTPUT_ENABLE, GPIOA);
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

static void write_long_enable()
{
	int k = 0;

	GPIO_PinState high_state = HAL_GPIO_ReadPin(GPIOB, WRITE_ENABLE);

	if (!high_state)
	{
		HAL_GPIO_TogglePin(GPIOB, WRITE_ENABLE);
	}

	while (k < 2)
	{
		HAL_GPIO_TogglePin(GPIOB, WRITE_ENABLE);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		delay_us(500);
		k++;
	}
}

static void write_enable()
{
	int k = 0;

	GPIO_PinState high_state = HAL_GPIO_ReadPin(GPIOB, WRITE_ENABLE);

	if (!high_state)
	{
		HAL_GPIO_TogglePin(GPIOB, WRITE_ENABLE);
	}

	while (k < 2)
	{
		HAL_GPIO_TogglePin(GPIOB, WRITE_ENABLE);
		delay_us(1);
		k++;
	}
}
