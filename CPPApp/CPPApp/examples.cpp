#include <iostream>
#include "usb.h"
#include <Windows.h>
#include <winusb.h>

void ConnectWithUSB_Device() 
{
    //void* pDeviceHandle;
    //WINUSB_INTERFACE_HANDLE interfaceHandle;

    //if (USB::Find_Device(0x04d8, 0x0052, &pDeviceHandle)) {

    //    //GetDeviceHandle

    //    //if (pDeviceHandle == INVALID_HANDLE_VALUE)
    //    //{
    //    //    return FALSE;
    //    //}

    //    //BOOL bResult = TRUE;

    //    //USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    //    //ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

    //    //WINUSB_PIPE_INFORMATION  Pipe;
    //    //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


    //    //bResult = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);
    //    //WINUSB_PIPE_INFORMATION  Pipe;

    //    //USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    //    //ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

    //    //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

    //    //bool result = false;

    //    //result = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);

    //    //result = WinUsb_QueryPipe(pDeviceHandle, 0, 1, &Pipe);

    //    if (WinUsb_Initialize(pDeviceHandle, &interfaceHandle)) {

    //        USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    //        ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

    //        //WINUSB_PIPE_INFORMATION  Pipe;
    //        //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


    //        //bResult = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);
    //        WINUSB_PIPE_INFORMATION  Pipe;

    //        /*USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
    //        ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));*/

    //        ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

    //        bool result = false;

    //        result = WinUsb_QueryInterfaceSettings(interfaceHandle, 0, &InterfaceDescriptor);

    //        result = WinUsb_QueryPipe(interfaceHandle, 0, 1, &Pipe);

    //        unsigned char buffer[66] = { 0 };
    //        unsigned long written = 0;

    //        buffer[0] = 1;
    //        buffer[1] = 7;
    //        buffer[2] = 7;
    //        buffer[3] = 7;
    //        buffer[4] = 7;
    //        buffer[5] = 7;
    //        buffer[6] = 7;
    //        buffer[7] = 7;
    //        buffer[8] = 7;
    //        buffer[9] = 7;
    //        buffer[10] = 7;
    //        buffer[11] = 7;
    //        buffer[62] = 6;
    //        buffer[63] = 6;

    //        int ss = 0;
    //        //WINUSB_SETUP_PACKET setup_packet = {
    //        //    USB_ENDPOINT_DIRECTION_IN(0),
    //        //    1,
    //        //    4,
    //        //    2,
    //        //    8
    //        //};
    //        //
    //        //WinUsb_ControlTransfer(interfaceHandle, setup_packet, buffer, 8, &written, NULL);

    //        while (true) {
    //            UCHAR data;
    //            std::cin >> data;
    //            buffer[0] = data;
    //            /*std::cout << "" << std::endl;*/
    //            /*buffer[0] = data;
    //            if (WinUsb_ReadPipe(interfaceHandle, 129, buffer, 3, &written, NULL)) {
    //                printf("");
    //            }
    //            ss = GetLastError();*/
    //            if (WinUsb_WritePipe(interfaceHandle, 1, buffer, 16, &written, NULL)) {
    //                std::cout << "Sent" << std::endl;
    //            }
    //        }
    //    }
    //}
}