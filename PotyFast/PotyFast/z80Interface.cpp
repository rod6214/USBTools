#include "z80Interface.h"

Z80_CONNECT::Z80Connector::Z80Connector(std::unique_ptr<CONNECT::USBConfig>& usb_ptr)
{
    usb = new std::unique_ptr<CONNECT::USB>(new CONNECT::USB(usb_ptr));
    
    if ((*usb)->USB_Init())
    {
        std::cout << "USB initialized" << std::endl;
    }
    else
    {
        std::cout << "USB error" << std::endl;
    }
}

void Z80_CONNECT::Z80Connector::Reset()
{
}

void Z80_CONNECT::Z80Connector::OneStep()
{
}

void Z80_CONNECT::Z80Connector::ProgramMode()
{
}

void Z80_CONNECT::Z80Connector::WriteMemory(const char* buffer, int bytes)
{
}

void Z80_CONNECT::Z80Connector::ReadMemory(const char* buffer, int bytes)
{
}

void Z80_CONNECT::Z80Connector::Run()
{
}

int Z80_CONNECT::Z80Connector::SendCommand(unsigned char command, const char* buffer, int bytes)
{
    unsigned char internal_buffer[66];
    if (bytes <= 48) 
    {
        internal_buffer[0] = command;
        internal_buffer[1] = (unsigned char)((bytes & 0xff00) >> 8);
        internal_buffer[2] = (unsigned char)(bytes & 0x00ff);
        unsigned char* pData = &internal_buffer[16];
        const char* src = buffer;
        for (int i = 0; i < bytes; i++) 
        {
            *(pData++) = *(src++);
        }

        CONNECT::USB_Data_t usb_data = {
            internal_buffer,
            bytes + 16,
            0,
            1
        };

        (*usb)->Write(usb_data);
        return usb_data.pCount;
    }

    return -1;
}

int Z80_CONNECT::Z80Connector::GetResponse(unsigned char* buffer, int bytes)
{
    if (bytes <= 48) 
    {
        CONNECT::USB_Data_t usb_data = {
            buffer,
            bytes + 16,
            0,
            1
        };

        (*usb)->Read(usb_data);
    }
    return 0;
}


