#include "usb.h"




#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	using namespace CONNECT;
	USB::USB(std::unique_ptr<USBConfig>& configs)
		: ptr_fns(configs), 
		ptr_sts(configs->GetStatus()), 
		interfaceHandle(nullptr)
	{}

	bool USB::Find_Device(USB_IDs_t& config)
	{
		GUID guid = config.guid;
		unsigned short p_VendorID = config.p_VendorID;
		unsigned short p_PoductID = config.p_PoductID;
		void** pDeviceHandle = &this->interfaceHandle;
		
		SP_DEVICE_INTERFACE_DATA myDeviceInterfaceData = {0};

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

	bool CONNECT::USB::USB_Init()
	{
		bool result = false;
		auto config = ptr_fns->GetConfig();

		std::string str_guid = config.get("guid", "UTF-32").asString();
		auto str_vid = config.get("vid", "UTF-32").asString();
		auto str_pid = config.get("pid", "UTF-32").asString();

		UTILS::Cstring _vid = str_vid;
		UTILS::Cstring _pid = str_pid;
		UTILS::Cstring _guid = str_guid;

		unsigned short vid = static_cast<unsigned short>(UTILS::Converter::ParseToHex(_vid));
		unsigned short pid = static_cast<unsigned short>(UTILS::Converter::ParseToHex(_vid));
		GUID guid = UTILS::Converter::ParseToGUID(_guid);

		USB_IDs_t ids = {
			guid,
			vid,
			pid
		};

		if (Find_Device(ids)) 
		{
			void** pDeviceHandle = &this->interfaceHandle;
			if (pDeviceHandle == INVALID_HANDLE_VALUE)
			{
				return FALSE;
			}

			BOOL bResult = TRUE;

			USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
			ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

			WINUSB_PIPE_INFORMATION  Pipe;
			ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

			result = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);

			result = WinUsb_QueryPipe(pDeviceHandle, 0, 1, &Pipe);
		}

		return result;
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
			new std::thread(&USBConfig::thread_callback, &(*ptr_fns)));
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

	USBConfig::USBConfig(std::unique_ptr<ThreadStatus> &pstatus, std::string& path) 
		: ptr_sts(pstatus), file_path(path)
	{}

	void USBConfig::thread_callback()
	{
		while(!this->ptr_sts->GetEndProcess()) 
		{
			std::cout << "test thread" << std::endl;
			Sleep(1000);
		}
	}

	std::string& USBConfig::GetPath()
	{
		return this->file_path;
	}

	Json::Value CONNECT::USBConfig::GetConfig()
	{
		Json::Value root;
		char* buffer = _getcwd(NULL, 0);
		std::string path(buffer == NULL ? "" : buffer);
		path.append("\\config.json");
		std::ifstream file(path, std::ifstream::binary);

		file >> root;

		std::string my_encoding = root.get("guid", "UTF-32").asString();
		UTILS::Cstring str = my_encoding;
		return root;
	}

	std::unique_ptr<ThreadStatus>& USBConfig::GetStatus()
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
