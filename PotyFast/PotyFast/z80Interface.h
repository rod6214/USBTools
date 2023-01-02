#pragma once
#include "ICPUConnector.h"
#include "framework.h"
#include "core.h"

namespace Z80_CONNECT 
{
	class DLLIMPORT Z80Connector : public ICPUConnector
	{
	public:
		Z80Connector(std::unique_ptr<CONNECT::USBConfig>& usb_ptr);
		virtual bool Reset() override;
		virtual bool OneStep() override;
		virtual bool ProgramMode() override;
		virtual CPUResponse WriteMemory(const char* buffer, int offset, int bytes) override;
		virtual CPUResponse ReadMemory(int offset, int bytes) override;
		virtual bool Run() override;
		const int USB_LIMIT_DATA = 64;
	protected:
		virtual int SendCommand(unsigned char command, const char* buffer, int offset, int bytes) override;
		virtual int GetResponse(char* buffer, int bytes) override;
	private:
		std::unique_ptr<CONNECT::USB>* usb;
		char* mem;
	};
}
