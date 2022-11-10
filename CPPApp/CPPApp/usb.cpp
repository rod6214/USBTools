#include "usb.h"
#include <combaseapi.h>
#include <SetupAPI.h>
#include <iostream>
#include <regex>
#include <memory>
#include <thread>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	USB::USB(std::unique_ptr<InternalFunctions>& functions)
		: ptr_fns(functions), ptr_sts(functions->GetStatus())
	{
	}

	bool USB::Find_Device(unsigned short p_VendorID,
							unsigned short p_PoductID,
							void** pDeviceHandle) 
	{
		//{1e680b0e-a57d-4f57-9941-02e013e3f4e5}
		//GUID guid = { 0x58D07210, 0x27C1, 0x11DD, 0xBD, 0x0B, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };
		//GUID guid = { 0xB3B0ED3C, 0x4DAE, 0x4878, 0x89, 0x6C, 0x80, 0x02, 0x0E, 0x17, 0x4E, 0x80 };
		GUID guid = { 0x1e680b0e, 0xa57d, 0x4f57, 0x99, 0x41, 0x02, 0xe0, 0x13, 0xe3, 0xf4, 0xe5 };
		SP_DEVICE_INTERFACE_DATA myDeviceInterfaceData = {0};
		//HANDLE pTempDeviceHandle;

		HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&guid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		if (INVALID_HANDLE_VALUE != deviceInfoSet) {
			myDeviceInterfaceData.cbSize = sizeof(myDeviceInterfaceData);

			for (int i = 0; i < 20; i++) {
			
				if (SetupDiEnumDeviceInterfaces(deviceInfoSet, 0, &guid, i, &myDeviceInterfaceData)) {

					unsigned long bufferSize = 0;
					wchar_t buffer[2048];

					SP_DEVICE_INTERFACE_DETAIL_DATA_W *detailDataBuffer = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)GlobalAlloc(GPTR, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W) * 1);
				
					if (detailDataBuffer != NULL) {

						SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &myDeviceInterfaceData, NULL, 0, &bufferSize, NULL);
						detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
						SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &myDeviceInterfaceData, detailDataBuffer, bufferSize, &bufferSize, NULL);
					
						int bytes = wsprintf(buffer, L"usb#vid_%04IX&pid_%04IX\0", (long long)p_VendorID, (long long)p_PoductID);

						std::wstring pid = buffer;
						std::wstring devicePath = detailDataBuffer->DevicePath;

						GlobalFree(detailDataBuffer);

						std::transform(pid.begin(), pid.end(), pid.begin(), ::tolower);

						if ((int)devicePath.find(pid) >= 0) {
							std::cout << "Device found! :)\n" << std::endl;

							*pDeviceHandle = CreateFile(devicePath.data(),
													GENERIC_READ | GENERIC_WRITE,
													FILE_SHARE_READ | FILE_SHARE_WRITE,
													NULL,
													OPEN_EXISTING,
													FILE_FLAG_OVERLAPPED,
													0);
							/**pDeviceHandle = CreateFile(devicePath.data(),
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								OPEN_EXISTING,
								0,
								0);*/

							bool validDeviceHandle = INVALID_HANDLE_VALUE != *pDeviceHandle;

							if (!validDeviceHandle)
								CloseHandle(*pDeviceHandle);

							return validDeviceHandle;
						}
					}	
				}
			}
		}

		return false;
	}

	USB::~USB()
	{
		this->ptr_fns.release();
		this->ptr_sts.release();
		this->ptr_thread.release();
	}

	void USB::BeginRead() 
	{
		this->ptr_thread = std::unique_ptr<std::thread>(
			new std::thread(&InternalFunctions::thread_callback, &(*ptr_fns)));
		this->ptr_sts->SetEndProcess(false);
	}

	void USB::EndRead() 
	{
		this->ptr_sts->SetEndProcess(true);
	}

	void USB::Wait()
	{
		this->ptr_thread->join();
	}

	InternalFunctions::InternalFunctions(std::unique_ptr<ThreadStatus> &pstatus):ptr_sts(pstatus)
	{}

	void InternalFunctions::thread_callback()
	{
		while(!this->ptr_sts->GetEndProcess()) 
		{
			std::cout << "test thread" << std::endl;
			Sleep(1000);
		}
	}

	std::unique_ptr<ThreadStatus>& InternalFunctions::GetStatus() 
	{
		return this->ptr_sts;
	}

	ThreadStatus::ThreadStatus():endProcess(false)
	{}

	bool ThreadStatus::GetEndProcess() 
	{
		return this->endProcess;
	}

	void ThreadStatus::SetEndProcess(bool value)
	{
		this->endProcess = value;
	}
#ifdef __cplusplus
}
#endif // __cplusplus
