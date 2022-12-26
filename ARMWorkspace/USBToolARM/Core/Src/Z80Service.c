/*
 * Z80Service.c
 *
 *  Created on: Dec 20, 2022
 *      Author: Nelson
 */
#include "Z80Service.h"

ActionResult_t results[6];


ActionResult_t* Z80_Reset()
{
	return &results[0];
}

ActionResult_t* Z80_OneStep()
{
	return &results[1];
}

ActionResult_t* Z80_ProgramMode()
{
	return &results[2];
}

ActionResult_t* Z80_WriteMemory(char* buffer, int offset, int bytes)
{
	return &results[3];
};

ActionResult_t* Z80_ReadMemory(int offset, int bytes)
{
	return &results[4];
}

ActionResult_t* Z80_Run()
{
	return &results[5];
}
