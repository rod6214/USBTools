#pragma once
#include "framework.h"

namespace Z80_CONNECT
{
	enum CARD_COMMANDS
	{
		WRITE_DATA = 0x78,
		WRITE_PACKAGE = 0x79,
		READ_DATA = 0x12,
		PROGRAM_MODE = 0x33,
		ONE_STEP = 0x73,
		RESET = 0x92,
		RUN_CPU = 0x65,
		ONE_STEP_CLOCK = 0x74,
		STATUS_PORT = 0x75
	};

	typedef struct _RESPONSE 
	{
		int bytes;
		char* buffer;
	} CPUResponse;

	typedef struct _STATUS 
	{
		bool busRequested;
		bool cpuModeRun;
		bool cpuModeOneStep;
	} CPU_STATUS_t;

	class ICPUConnector
	{
	public:
		virtual bool Reset() = 0;
		virtual bool OneStep() = 0;
		virtual bool ProgramMode() = 0;
		virtual bool OneStepClock() = 0;
		virtual CPU_STATUS_t Status() = 0;
		virtual CPUResponse WriteMemoryPackage(const char* buffer, int offset, int bytes) = 0;
		virtual CPUResponse WriteMemory(const char* buffer, int offset, int bytes) = 0;
		virtual CPUResponse ReadMemory(int offset, int bytes) = 0;
		virtual bool Run() = 0;
	protected:
		virtual int SendCommand(unsigned char command, const char* buffer, int offset, int bytes) = 0;
		virtual int GetResponse(char* buffer, int bytes) = 0;
	};
}