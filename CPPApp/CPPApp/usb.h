#pragma once
#ifndef CUSTOM_USB_H
#define CUSTOM_USB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	class USB 
	{
		public:
			static bool Find_Device(unsigned short p_VendorID,
									unsigned short p_PoductID,
									void** pDeviceHandle);
	};
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !CUSTOM_USB_H
