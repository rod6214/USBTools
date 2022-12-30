#pragma once

#ifndef CORE_USB_H
#define CORE_USB_H
//#pragma comment(lib, "jsoncpp.lib") 

#include "json/json.h"
#include "framework.h"
#include "core.h"

#ifdef __cplusplus
extern "C"
{
#endif

	namespace CONNECT
	{
		typedef struct _USB_BODY 
		{
			char* buffer;
			unsigned long buffer_len;
			unsigned long pCount;
			unsigned char endpoint;
		} USB_Data_t;

		typedef struct _USB_DATA
		{
			GUID guid;
			unsigned short p_VendorID;
			unsigned short p_PoductID;
		} USB_IDs_t;

		class DLLIMPORT ThreadStatus
		{
		public:
			ThreadStatus();
			bool GetEndProcess();
			void SetEndProcess(bool value);
		private:
			bool endProcess;
		};

		class DLLIMPORT USBConfig
		{
		public:
			USBConfig(std::unique_ptr<ThreadStatus>& pstatus, std::string& path);
			void thread_callback();
			std::unique_ptr<ThreadStatus>& GetStatus();
			std::string& GetPath();
			Json::Value GetConfig();
		private:
			std::unique_ptr<ThreadStatus>& ptr_sts;
			std::string& file_path;
		};

		class DLLIMPORT USB
		{
		public:
			USB(std::unique_ptr<USBConfig>& configs);
			void BeginRead();
			void EndRead();
			void Wait();
			bool USB_Init();
			bool Write(USB_Data_t& data);
			bool Read(USB_Data_t& data);
			~USB();
		private:
#pragma warning(push)
#pragma warning(disable:4251)
			std::unique_ptr<USBConfig>& ptr_fns;
			std::unique_ptr<ThreadStatus>& ptr_sts;
			std::unique_ptr<std::thread> ptr_thread;
#pragma warning(pop)
			WINUSB_INTERFACE_HANDLE interfaceHandle;
			void* pDeviceHandle;
			bool Find_Device(USB_IDs_t& config);
		};
	}

	namespace UTILS
	{
#pragma warning(push)
#pragma warning(disable:4251)
		class DLLIMPORT Cstring : public std::string
		{
		public:
			Cstring();
			Cstring(std::string& value);
			Cstring(const char* value);
			std::vector<Cstring> split(char delimiter);
#pragma warning(pop)
		};

		class DLLIMPORT Directory
		{
		public:
			static UTILS::Cstring GetAppDirectory();
		};

		class DLLIMPORT Converter
		{
		public:
			static int ParseInt(UTILS::Cstring& value);
			static long ParseLong(UTILS::Cstring& value);
			static unsigned long long ParseToHex(UTILS::Cstring& value);
			static GUID ParseToGUID(UTILS::Cstring& value);
		};
	}
#ifdef __cplusplus
}
#endif
#endif