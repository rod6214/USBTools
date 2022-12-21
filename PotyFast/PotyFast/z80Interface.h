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
		virtual void Reset() override;
		virtual void OneStep() override;
		virtual void ProgramMode() override;
		virtual void WriteMemory(const char* buffer, int bytes) override;
		virtual void ReadMemory(const char* buffer, int bytes) override;
		virtual void Run() override;
	protected:
		virtual int SendCommand(unsigned char command, const char* buffer, int bytes) override;
		virtual int GetResponse(unsigned char* buffer, int bytes) override;
	private:
		std::unique_ptr<CONNECT::USB>* usb;
	};
}
