/*
 * Z80Service.c
 *
 *  Created on: Dec 20, 2022
 *      Author: Nelson
 */
#include "Z80Service.h"
#include <memory.h>
#include "PortController.h"

ActionResult_t results[6];


ActionResult_t* Z80_Reset()
{
	results[3].bytes = 0;
	Port_ResetCPU();
	return &results[0];
}

ActionResult_t* Z80_OneStep()
{
	results[3].bytes = 0;
	Port_Reset();
	Port_OnStep();
	return &results[1];
}

ActionResult_t* Z80_ProgramMode()
{
	results[3].bytes = 0;
	Port_ProgramCPU();
	return &results[2];
}

ActionResult_t* Z80_Run()
{
	results[3].bytes = 0;
	Port_Reset();
	Port_RunCPU();
	return &results[5];
}

ActionResult_t* Z80_WriteMemory(char* buffer, int offset, int bytes)
{
	if (bytes <= 48)
	{
		const char* response = "TEST WRITE";
		results[3].bytes = strlen(response);
		Port_Write(buffer, offset, bytes);
		strcpy(results[3].buffer, response);
	}
	return &results[3];
};

ActionResult_t* Z80_ReadMemory(int offset, int bytes)
{
	if (bytes <= 48)
	{
		char buffer[48];
		Port_Read(buffer, offset, bytes);
		results[4].bytes = bytes;
		memcpy(results[4].buffer, buffer, bytes);
	}
	return &results[4];
}
