#include "framework.h"
#include "core.h"
//DLLIMPORT void test()
//{
//	std::cout << "Hello" << std::endl;
//}

namespace CONNECT
{
	USB::USB(std::unique_ptr<USBConfig>& configs)
		: ptr_fns(configs),
		ptr_sts(configs->GetStatus()),
		interfaceHandle(nullptr),
		pDeviceHandle(nullptr)
	{}

	bool USB::Find_Device(USB_IDs_t& config)
	{
		GUID guid = config.guid;
		unsigned short p_VendorID = config.p_VendorID;
		unsigned short p_PoductID = config.p_PoductID;
		void** pDeviceHandle = &this->pDeviceHandle;

		SP_DEVICE_INTERFACE_DATA myDeviceInterfaceData = { 0 };

		HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&guid, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		if (INVALID_HANDLE_VALUE != deviceInfoSet) {
			myDeviceInterfaceData.cbSize = sizeof(myDeviceInterfaceData);

			for (int i = 0; i < 20; i++) {

				if (SetupDiEnumDeviceInterfaces(deviceInfoSet, 0, &guid, i, &myDeviceInterfaceData)) {

					unsigned long bufferSize = 0;
					wchar_t buffer[2048];

					SP_DEVICE_INTERFACE_DETAIL_DATA_W* detailDataBuffer = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)GlobalAlloc(GPTR, sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W) * 1);

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
							//std::cout << "Device found! :)\n" << std::endl;

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

	bool USB::USB_Init()
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
		unsigned short pid = static_cast<unsigned short>(UTILS::Converter::ParseToHex(_pid));
		GUID guid = UTILS::Converter::ParseToGUID(_guid);

		USB_IDs_t ids = {
			guid,
			vid,
			pid
		};

		if (Find_Device(ids))
		{
			void* pDeviceHandle = this->pDeviceHandle;

			if (pDeviceHandle == INVALID_HANDLE_VALUE)
			{
				return FALSE;
			}

			PWINUSB_INTERFACE_HANDLE pinterfaceHandle = &this->interfaceHandle;

			if (WinUsb_Initialize(pDeviceHandle, pinterfaceHandle))
			{
				USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
				ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

				WINUSB_PIPE_INFORMATION Pipe;
				ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

				result = WinUsb_QueryInterfaceSettings(*pinterfaceHandle, 0, &InterfaceDescriptor);

				result = WinUsb_QueryPipe(*pinterfaceHandle, 0, 1, &Pipe);
			}
		}

		return result;
	}

	bool USB::Write(USB_Data_t& data)
	{
		if (data.buffer_len > 64)
		{
			const char* error = "Error: More than 64 bytes are not permitted.";
			throw error;
		}
		unsigned long written = 0;
		//unsigned char buffer[66] = {0};
		auto result = WinUsb_WritePipe(interfaceHandle, 
			1, data.buffer, 63, &written, NULL);
		return result;
	}

	bool USB::Read(USB_Data_t& data)
	{
		if (data.buffer_len > 64)
		{
			const char* error = "Error: More than 64 bytes are not permitted.";
			throw error;
		}
		auto result = WinUsb_ReadPipe(interfaceHandle, 128 | data.endpoint, 
			data.buffer, data.buffer_len, data.pCount, NULL);
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

	USBConfig::USBConfig(std::unique_ptr<ThreadStatus>& pstatus, std::string& path)
		: ptr_sts(pstatus), file_path(path)
	{}

	void USBConfig::thread_callback()
	{
		while (!this->ptr_sts->GetEndProcess())
		{
			std::cout << "test thread" << std::endl;
			Sleep(1000);
		}
	}

	std::string& USBConfig::GetPath()
	{
		return this->file_path;
	}

	Json::Value USBConfig::GetConfig()
	{
		Json::Value root;
		std::string path(this->GetPath());
		std::ifstream file(path, std::ifstream::binary);
		file >> root;
		return root;
	}

	std::unique_ptr<ThreadStatus>& USBConfig::GetStatus()
	{
		return this->ptr_sts;
	}

	ThreadStatus::ThreadStatus() :endProcess(false)
	{}

	bool ThreadStatus::GetEndProcess()
	{
		return this->endProcess;
	}

	void ThreadStatus::SetEndProcess(bool value)
	{
		this->endProcess = value;
	}
}

namespace UTILS 
{
	Cstring::Cstring() {}

	Cstring::Cstring(std::string& value)
	{
		this->assign(value);
	}

	Cstring::Cstring(const char* value)
	{
		this->assign(value);
	}

	std::vector<Cstring> Cstring::split(char delimiter)
	{
		std::vector<UTILS::Cstring> strings;
		std::istringstream in(this->data());
		std::string s;

		while (std::getline(in, s, delimiter))
		{
			strings.push_back(s);
		}
		return strings;
	}

	int Converter::ParseInt(UTILS::Cstring& value)
	{
		auto result = std::stoi(value);
		return result;
	}

	long Converter::ParseLong(UTILS::Cstring& value)
	{
		auto result = std::stol(value);
		return result;
	}

	unsigned long long Converter::ParseToHex(UTILS::Cstring& value)
	{
		unsigned long long x;
		std::stringstream in;
		in << std::hex << value;
		in >> x;
		auto result = static_cast<unsigned long long>(x);
		return result;
	}

	GUID Converter::ParseToGUID(UTILS::Cstring& value)
	{
		GUID guid;
		std::vector<UTILS::Cstring> codes = value.split('-');
		int len = static_cast<int>(codes.size());

		guid.Data1 = static_cast<unsigned long>(Converter::ParseToHex(codes[0]));
		guid.Data2 = static_cast<unsigned short>(Converter::ParseToHex(codes[1]));
		guid.Data3 = static_cast<unsigned short>(Converter::ParseToHex(codes[2]));

		unsigned short ptrMSB = static_cast<unsigned short>(Converter::ParseToHex(codes[3]));
		unsigned long long ptrLSB = Converter::ParseToHex(codes[4]);

		unsigned char* ptr = guid.Data4;
		unsigned char* psrc = static_cast<unsigned char*>((void*)&ptrMSB);
		psrc++;
		for (int i = 2; i > 0; i--)
		{
			*(ptr++) = *(psrc--);
		}

		psrc = static_cast<unsigned char*>((void*)&ptrLSB);
		psrc += 5;

		for (int i = len; i >= 0; i--)
		{
			*(ptr++) = *(psrc--);
		}

		return guid;
	}

	Cstring Directory::GetAppDirectory()
	{
		char* buffer = _getcwd(NULL, 0);
		UTILS::Cstring path(buffer == NULL ? "" : buffer);
		return path;
	}
}
