#include "z80Interface.h"

Z80_CONNECT::Z80Connector::Z80Connector(std::unique_ptr<CONNECT::USBConfig>& usb_ptr)
{
    usb = new std::unique_ptr<CONNECT::USB>(new CONNECT::USB(usb_ptr));
    mem = new char[65536];
    //buffer = new std::unique_ptr<std::stringbuf>(new std::stringbuf());
    
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

bool Z80_CONNECT::Z80Connector::OneStepClock()
{
    char result[64];
    char* str = new char[64];
    SendCommand(ONE_STEP_CLOCK, NULL, 0, 0);
    int res = GetResponse(result, 0);
    memcpy(str, result, res);
    if (res != 0)
    {
        if (strcmp(str, "ONE_STEP_CLOCK") == 0)
            return true;
    }
    return false;
}

Z80_CONNECT::CPU_STATUS_t Z80_CONNECT::Z80Connector::Status()
{
    char result[64];
    SendCommand(STATUS_PORT, NULL, 0, 3);
    int res = GetResponse(result, 0);
    if (res != 0)
    {
        if (strcmp(result, "STATUS_PORT") == 0)
        {
            bool busRequested = false;
            bool cpuModeRun = false;
            bool cpuModeOneStep = false;

            if (!result[16]) busRequested = true;
            if (result[17]) cpuModeOneStep = true;
            if (result[18]) cpuModeRun = true;

            return { busRequested, cpuModeRun, cpuModeOneStep };
        }
    }
    throw "Bad response from device.";
}

Z80_CONNECT::CPUResponse Z80_CONNECT::Z80Connector::WriteMemory(const char* buffer, int offset, int bytes) 
{
    char result[64];
    int row = bytes <= 48 ? 1 : (bytes / 48) + 1;
    int totalBytes = bytes;

    for (int i = 0; i < row; i++) 
    {
        if (bytes <= 48) 
        {
            SendCommand(WRITE_DATA, buffer, offset, totalBytes);
        }
        else 
        {
            int k = i * 48;
            SendCommand(WRITE_DATA, &buffer[k], (offset + k), 48);
            totalBytes -= 48;
        }

        int res = GetResponse(result, 0);

        if (res != 0)
        {
            if (strcmp(result, "WRITE_DATA") != 0)
                throw "Bad response from device.";
        }
    }

    return { bytes };
}

Z80_CONNECT::CPUResponse Z80_CONNECT::Z80Connector::WriteMemoryPackage(const char* buffer, int offset, int bytes)
{
    char result[64];
    int row = bytes < USB_LIMIT_DATA ?  1 : bytes / USB_LIMIT_DATA;
    int totalBytes = bytes;
    int z = offset;

    for (int i = 0; i < row; i++)
    {
        int j = 0, res = 0;

        while (j < 2) 
        {
            int k = j * 32;
            SendCommand(WRITE_PACKAGE, &buffer[k], offset, 32);
            res = GetResponse(result, 0);
            z += 32;
            j++;
        }

        if (res != 0)
        {
            if (strcmp(result, "WRITE_PACKAGE") != 0)
                throw "Bad response from device.";
        }
    }

    return { bytes };
}

Z80_CONNECT::CPUResponse Z80_CONNECT::Z80Connector::ReadMemory(int offset, int bytes)
{
    char result[64];
    int row = bytes <= USB_LIMIT_DATA ? 1 : (bytes / USB_LIMIT_DATA + 1);
    int totalBytes = bytes;
    char* ptr = mem;
    int k = offset;
    
    for (int i = 0; i < row; i++)
    {
        int z = 0, res = 0;

        while (z < 2) 
        {
            SendCommand(READ_DATA, NULL, k, 32);
            res = GetResponse(result, 0);

            if (res != 0)
            {
                for (int j = 0; j < 32; j++)
                {
                    *(ptr++) = result[j + 16];
                }

                if (strcmp(result, "READ_DATA") != 0)
                    throw "Bad response from device.";
            }

            k += 32;
            z++;
        }
    }

    return { bytes, mem };
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


