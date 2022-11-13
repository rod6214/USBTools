#pragma once
#ifndef CUSTOM_USB_H
#define CUSTOM_USB_H
#include <combaseapi.h>
#include <SetupAPI.h>
#include <iostream>
#include <regex>
#include <memory>
#include <thread>
#include <windows.data.json.h>
#include <string>
#include <vector>

#include <Windows.h>
#include <winusb.h>
#include <future>
//#include <json/json.h>
#include <stdlib.h>
#include <string>
#include <direct.h>
#include <fstream>

#pragma comment(lib, "jsoncpp.lib") 
#include "json/json.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	namespace CONNECT 
	{
		typedef struct _USB_DATA 
		{
			GUID guid;
			unsigned short p_VendorID;
			unsigned short p_PoductID;
		} USB_IDs_t;

		class ThreadStatus
		{
		public:
			ThreadStatus();
			bool GetEndProcess();
			void SetEndProcess(bool value);
		private:
			bool endProcess;
		};

		class USBConfig
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

		class USB
		{
		public:
			USB(std::unique_ptr<USBConfig>& configs);
			void BeginRead();
			void EndRead();
			void Wait();
			bool USB_Init();
			bool Write(
				unsigned char* buffer, 
				unsigned long buffer_len,
				unsigned long* written);
			~USB();
		private:
			std::unique_ptr<USBConfig>& ptr_fns;
			std::unique_ptr<ThreadStatus>& ptr_sts;
			std::unique_ptr<std::thread> ptr_thread;
			WINUSB_INTERFACE_HANDLE interfaceHandle;
			bool Find_Device(USB_IDs_t& config);
			/*bool Find_Device(
				GUID guid,
				unsigned short p_VendorID,
				unsigned short p_PoductID,
				void** pDeviceHandle);*/
		};
	}

	namespace UTILS 
	{
		class Cstring : public std::string
		{
		public:
			Cstring();
			Cstring(std::string& value);
			std::vector<Cstring> split(char delimiter);
		};

		class Converter 
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
#endif // __cplusplus


#endif // !CUSTOM_USB_H
