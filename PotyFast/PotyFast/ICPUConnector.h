#pragma once
#include "framework.h"

namespace Z80_CONNECT
{
	enum CARD_COMMANDS
	{
		WRITE_DATA = 0x78,
		READ_DATA = 0x12,
		PROGRAM_MODE = 0x33,
		ONE_STEP = 0x73,
		RESET = 0x92,
		RUN_CPU = 0x65
	};

	class ICPUConnector
	{
	public:
		virtual void Reset() = 0;
		virtual void OneStep() = 0;
		virtual void ProgramMode() = 0;
		virtual void WriteMemory(const char* buffer, int bytes) = 0;
		virtual void ReadMemory(const char* buffer, int bytes) = 0;
		virtual void Run() = 0;
	protected:
		virtual int SendCommand(unsigned char command, const char* buffer, int bytes) = 0;
		virtual int GetResponse(unsigned char* buffer, int bytes) = 0;
	};
}