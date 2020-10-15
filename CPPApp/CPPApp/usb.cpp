#include "usb.h"
#include <combaseapi.h>
#include <SetupAPI.h>
#include <iostream>
#include <regex>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

bool USB::Find_Device(unsigned short p_VendorID,
						unsigned short p_PoductID,
						void** pDeviceHandle) 
{
	GUID guid = { 0x58D07210, 0x27C1, 0x11DD, 0xBD, 0x0B, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66 };
	SP_DEVICE_INTERFACE_DATA myDeviceInterfaceData = {0};
	HANDLE pTempDeviceHandle;

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

#ifdef __cplusplus
}
#endif // __cplusplus