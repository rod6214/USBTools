#pragma once
#ifndef CUSTOM_USB_H
#define CUSTOM_USB_H
#include <memory>
#include <thread>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	class ThreadStatus
	{
	public:
		ThreadStatus();
		bool GetEndProcess();
		void SetEndProcess(bool value);
	private:
		bool endProcess;
	};

	class InternalFunctions
	{
	public:
		InternalFunctions(std::unique_ptr<ThreadStatus> &pstatus);
		void thread_callback();
		std::unique_ptr<ThreadStatus>& GetStatus();
	private:
		std::unique_ptr<ThreadStatus> &ptr_sts;
	};

	class USB 
	{
		public:
			USB(std::unique_ptr<InternalFunctions>& functions);
			static bool Find_Device(unsigned short p_VendorID,
									unsigned short p_PoductID,
									void** pDeviceHandle);
			void BeginRead();
			void EndRead();
			void Wait();
			~USB();
	private:
		std::unique_ptr<InternalFunctions> &ptr_fns;
		std::unique_ptr<ThreadStatus> &ptr_sts;
		std::unique_ptr<std::thread> ptr_thread;
	};
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !CUSTOM_USB_H
