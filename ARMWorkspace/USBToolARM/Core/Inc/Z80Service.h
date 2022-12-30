/*
 * Z80Service.h
 *
 *  Created on: Dec 20, 2022
 *      Author: Nelson
 */

#ifndef INC_Z80SERVICE_H_
#define INC_Z80SERVICE_H_

enum CARD_COMMANDS
	{
		WRITE_DATA = 0x78,
		READ_DATA = 0x12,
		PROGRAM_MODE = 0x33,
		ONE_STEP = 0x73,
		CRESET = 0x92,
		RUN_CPU = 0x65
	};

typedef struct Z80_ACTION_RESULT
{
	char buffer[48];
	int bytes;
} ActionResult_t;

extern ActionResult_t* Z80_Reset();
extern ActionResult_t* Z80_OneStep();
extern ActionResult_t* Z80_ProgramMode();
extern ActionResult_t* Z80_WriteMemory(char* buffer, int offset, int bytes);
extern ActionResult_t* Z80_ReadMemory(int offset, int bytes);
extern ActionResult_t* Z80_Run();

#endif /* INC_Z80SERVICE_H_ */
