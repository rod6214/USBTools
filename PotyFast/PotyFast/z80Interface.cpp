#include "z80Interface.h"

Z80_CONNECT::Z80Connector::Z80Connector(std::unique_ptr<CONNECT::USBConfig>& usb_ptr)
{
    usb = new std::unique_ptr<CONNECT::USB>(new CONNECT::USB(usb_ptr));
    
    if ((*usb)->USB_Init())
    {
        std::cout << "USB initialized." << std::endl;
    }
    else
    {
        throw "USB error or divice is not connected.";
    }
}

bool Z80_CONNECT::Z80Connector::Reset()
{
    char result[64];
    char str[64];
    SendCommand(RESET, NULL, 0, 0);
    int res = GetResponse(result, 0);
    
    if (res < 0)
        return false;

    memcpy(str, result, res);
    
    str[63] = '\0';

    if (strcmp(str, "RESET") == 0)
        return true;
    
    return false;
}

bool Z80_CONNECT::Z80Connector::OneStep()
{
    char result[64];
    char str[64];
    SendCommand(ONE_STEP, NULL, 0, 0);
    int res = GetResponse(result, 0);

    if (res < 0)
        return false;

    memcpy(str, result, res);

    str[63] = '\0';

    if (strcmp(str, "ONE_STEP") == 0)
        return true;

    return false;
}

bool Z80_CONNECT::Z80Connector::ProgramMode()
{
    char result[64];
    char* str = new char[64];
    SendCommand(PROGRAM_MODE, NULL, 0, 0);
    int res = GetResponse(result, 0);
    if (res != 0)
    {
        return true;
    }
    return false;
}

Z80_CONNECT::CPUResponse Z80_CONNECT::Z80Connector::WriteMemory(const char* buffer, int offset, int bytes)
{
    char result[64];
    char* str = new char[64];
    SendCommand(WRITE_DATA, buffer, offset, bytes);
    int res = GetResponse(result, 0);

    if (res != 0) 
    {
        memcpy(str, &result[16], 48);

        str[63] = '\0';

        if (strcmp(result, "WRITE_DATA") == 0)
            return { result[15],  str };
    }

    return {0};
}

Z80_CONNECT::CPUResponse Z80_CONNECT::Z80Connector::ReadMemory(int offset, int bytes)
{
    char result[64];
    char* str = new char[64];
    SendCommand(READ_DATA, NULL, offset, bytes);
    int res = GetResponse(result, 0);

    if (res != 0)
    {
        memcpy(str, &result[16], 48);

        str[63] = '\0';

        if (strcmp(result, "READ_DATA") == 0)
            return { result[15],  str };
    }

    return { 0 };
}

bool Z80_CONNECT::Z80Connector::Run()
{
    char result[64];
    char* str = new char[64];
    SendCommand(RUN_CPU, NULL, 0, 0);
    int res = GetResponse(result, 0);
    if (res != 0)
    {
        return true;
    }
    return false;
}

int Z80_CONNECT::Z80Connector::SendCommand(unsigned char command, const char* buffer, int offset, int bytes)
{
    char internal_buffer[66];
    if (bytes <= 48) 
    {
        internal_buffer[0] = command;
        internal_buffer[1] = (unsigned char)((bytes & 0xff00) >> 8);
        internal_buffer[2] = (unsigned char)(bytes & 0x00ff);
        internal_buffer[3] = (unsigned char)((offset & 0xff00) >> 8);
        internal_buffer[4] = (unsigned char)(offset & 0x00ff);
        
        if (buffer != NULL) 
        {
            char* pData = &internal_buffer[16];
            const char* src = buffer;
            for (int i = 0; i < bytes; i++)
            {
                *(pData++) = *(src++);
            }
        }

        CONNECT::USB_Data_t usb_data = {
            internal_buffer,
            64,
            0,
            1
        };

        (*usb)->Write(usb_data);
        return usb_data.pCount;
    }

    return -1;
}

int Z80_CONNECT::Z80Connector::GetResponse(char* buffer, int bytes)
{
    if (bytes <= 48) 
    {
        CONNECT::USB_Data_t usb_data = {
            buffer,
            64,
            0,
            1
        };

        (*usb)->Read(usb_data);
        return usb_data.pCount;
    }
    return -1;
}


