/*
 * Z80Service.c
 *
 *  Created on: Dec 20, 2022
 *      Author: Nelson
 */
#include "Z80Service.h"
#include <memory.h>
#include "PortController.h"

ActionResult_t results[6] = {0};
const int CMAX_PACKET_SIZE = 64;


ActionResult_t* Z80_Reset()
{
	results[0].bytes = 0;
	Port_ResetCPU();
	return &results[0];
}

ActionResult_t* Z80_OneStep()
{
	results[1].bytes = 0;
	Port_OneStep();
	return &results[1];
}

ActionResult_t* Z80_ProgramMode()
{
	results[2].bytes = 0;
	Port_ProgramCPU();
	return &results[2];
}

ActionResult_t* Z80_Status()
{
	results[2].bytes = 3;
	Port_Status(results[2].buffer);
	return &results[2];
}

ActionResult_t* Z80_OneStepPulse() 
{
	results[2].bytes = 0;
	Port_OneStepClock();
	return &results[2];
}

ActionResult_t* Z80_Run()
{
	results[3].bytes = 0;
	Port_RunCPU();
	return &results[3];
}

char* ptr;

ActionResult_t* Z80_WriteMemoryPackage(char* buffer, int offset, int bytes)
{
	if (!results[3].started)
		ptr = results[3].inputBuffer;

	for (int i = 0; i < bytes; i++)
	{
		*(ptr++) = buffer[i];
	}

	if (results[3].started)
	{
		const char* response = "NEXT";
		results[3].started = 0;
		strcpy(results[3].buffer, response);
		Port_Write(results[3].inputBuffer, offset, 64);
	}
	else
	{
		const char* response = "FIRST";
		strcpy(results[3].buffer, response);
		results[3].started++;
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

ActionResult_t* Z80_WriteMemory(char* buffer, int offset, int bytes)
{
	const char* response = "DONE";
	strcpy(results[3].buffer, response);
	Port_Write(buffer, offset, bytes);
	return &results[3];
}
