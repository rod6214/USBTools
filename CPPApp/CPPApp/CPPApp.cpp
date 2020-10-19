// CPPApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "usb.h"
#include <Windows.h>
#include <winusb.h>

int main()
{
    void *pDeviceHandle;
    WINUSB_INTERFACE_HANDLE interfaceHandle;

    if (USB::Find_Device(0x04d8, 0x0053, &pDeviceHandle)) {

        //GetDeviceHandle

        //if (pDeviceHandle == INVALID_HANDLE_VALUE)
        //{
        //    return FALSE;
        //}

        //BOOL bResult = TRUE;

        //USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
        //ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

        //WINUSB_PIPE_INFORMATION  Pipe;
        //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


        //bResult = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);
        //WINUSB_PIPE_INFORMATION  Pipe;

        //USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
        //ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

        //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

        //bool result = false;

        //result = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);

        //result = WinUsb_QueryPipe(pDeviceHandle, 0, 1, &Pipe);

        if (WinUsb_Initialize(pDeviceHandle, &interfaceHandle)) {

            USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
            ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

            //WINUSB_PIPE_INFORMATION  Pipe;
            //ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


            //bResult = WinUsb_QueryInterfaceSettings(pDeviceHandle, 0, &InterfaceDescriptor);
            WINUSB_PIPE_INFORMATION  Pipe;

            /*USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
            ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));*/

            ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

            bool result = false;

            result = WinUsb_QueryInterfaceSettings(interfaceHandle, 0, &InterfaceDescriptor);

            result = WinUsb_QueryPipe(interfaceHandle, 0, 2, &Pipe);

            unsigned char buffer[66] = {0};
            unsigned long written = 0;

            buffer[0] = 1;
            buffer[1] = 7;
            buffer[2] = 7;
            buffer[3] = 7;
            buffer[4] = 7;
            buffer[5] = 7;
            buffer[6] = 7;
            buffer[7] = 7;
            buffer[8] = 7;
            buffer[9] = 7;
            buffer[10] = 7;
            buffer[11] = 7;
            buffer[62] = 6;
            buffer[63] = 6;

            int ss = 0;

            while (true) {
                if (WinUsb_ReadPipe(interfaceHandle, 129, buffer, 64, &written, NULL)) {
                    if (buffer[0] == 7) {
                        buffer[0] = 0;
                    }
                }
                ss = GetLastError();
                //if (WinUsb_WritePipe(interfaceHandle, 1, buffer, 64, &written, NULL)) {
                //    std::cout << "dd" << std::endl;
                //}
            }
        }
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
