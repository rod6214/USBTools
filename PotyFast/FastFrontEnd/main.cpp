#include "Generic.h"
#include <streambuf>
#include <sstream>
#include <iostream>
#include <fstream>

typedef struct _CONFIG_STEP
{
    std::unique_ptr<CONNECT::ThreadStatus>& sts;
    UTILS::Cstring path;
} MainConfig_t;

typedef struct _MAIN_ARGS 
{
    int offset;
    int bytes;
    bool isWrite;
    bool isFile;
    const char* path;
    int fileOffset;
    bool isRam;
    bool isRom;
    int value;
} MainArgs_t;

MainConfig_t usb_config();
MainArgs_t process_args(int args, const char* argv[]);

int main(int args, const char* argv[])
{   
    std::cout << "Start" << std::endl;
    auto mainArgs = process_args(args, argv);

    std::cout << mainArgs.bytes << std::endl;

    if ((mainArgs.offset > 65535) || (mainArgs.bytes > 65536))
    {
        throw "Offset is out of range.";
    }

    if ((mainArgs.offset + mainArgs.bytes) > 65536) 
    {
        throw "There was an overflow, please check your parameters.";
    }

    if (mainArgs.bytes > 0)
    {
        try 
        {
            auto config = usb_config();
            auto usb_conf = std::unique_ptr<CONNECT::USBConfig>(new CONNECT::USBConfig(config.sts, config.path));
            Z80_CONNECT::Z80Connector z80card(usb_conf);
            std::stringstream data_mesh;

            if (mainArgs.isWrite && mainArgs.isRam && (mainArgs.value >= 0))
            {
                char data[1];
                data[0] = 0xff & mainArgs.value;
                z80card.WriteMemory(data, mainArgs.offset, 1);
            }
            else if (mainArgs.isWrite)
            {       
                if (((mainArgs.bytes % 64) != 0) && mainArgs.isRom)
                {
                    throw "Number of bytes should be multiple of 64.";
                }

                if (((mainArgs.offset % 64) != 0) && mainArgs.isRom)
                {
                    throw "Offset should be multiple of 64 or 0.";
                }

                std::fstream file;
                file.open(mainArgs.path, std::ios::binary | std::ios::in);

                char* binary = new char[mainArgs.bytes];
                char* ptr = binary;
                int val = 0, len = 0;

                if (mainArgs.fileOffset > 0) 
                {
                    int index = mainArgs.fileOffset;
                    file.seekg(index);
                }

                file.read(binary, mainArgs.bytes);

                file.close();

                if (mainArgs.isRom)
                    z80card.WriteMemoryPackage(binary, mainArgs.offset, mainArgs.bytes);

                if (mainArgs.isRam)
                    z80card.WriteMemory(binary, mainArgs.offset, mainArgs.bytes);

                delete[] binary;
            }
            else
            {
                auto value = z80card.ReadMemory(mainArgs.offset, mainArgs.bytes);
                int row = mainArgs.offset;
                data_mesh << "     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" << std::endl;
                for (int i = 0; i < value.bytes; i++) 
                {
                    int cell = 0xff & value.buffer[i];
                    if ((i % 16) == 0) 
                    {
                        if (i == 0)
                            data_mesh << "     -----------------------------------------------" << std::endl;
                        else
                            data_mesh << std::endl;

                        if (row < 0x10)
                            data_mesh << "000" << std::hex << row;
                        else if (row < 0x100)
                            data_mesh << "00" << std::hex << row;
                        else if (row < 0x1000)
                            data_mesh << "0" << std::hex << row;
                        else
                            data_mesh << std::hex << row;
                        data_mesh << " ";
                        row+=16;
                    }

                    if (cell < 0x10)
                        data_mesh << "0" << std::hex << (0xff & (int)value.buffer[i]) << " ";
                    else
                        data_mesh << std::hex << (0xff & (int)value.buffer[i]) << " ";
                }

                data_mesh << std::endl;
            }

            std::cout << data_mesh.str() << "\0" << std::endl;
        }
        catch (const char* err) 
        {
            std::cout << err << std::endl;
        }
    }
}

MainConfig_t usb_config()
{
    auto status = std::unique_ptr<CONNECT::ThreadStatus>(new CONNECT::ThreadStatus());
    auto path = UTILS::Directory::GetAppDirectory();
    path.append("\\config.json");
    CONNECT::USBConfig config(status, path);

    MainConfig_t cng = {
        status,
        path
    };

    return cng;
}

MainArgs_t process_args(int args, const char* argv[]) 
{
    int bytes = 0, offset = 0, fileOffset = 0, value = -1;
    bool isWrite = false, isFile = false;
    const char* path = "";
    bool hasBytes = false, isRam = false, isRom = false;

    for (int i = 0; i < args; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            std::stringstream temp;
            temp << argv[i + 1];
            temp >> offset;

            if (offset == 0)
            {
                temp.str("");
                temp << std::hex << argv[i + 1];
                temp >> offset;
            }
        }

        if (strcmp(argv[i], "-b") == 0)
        {
            hasBytes = true;
            std::stringstream temp;
            temp << argv[i + 1];
            temp >> bytes;

            if (bytes == 0)
            {
                temp.str("");
                temp << std::hex << argv[i + 1];
                temp >> bytes;
            }
        }

        if (strcmp(argv[i], "-w") == 0) 
        {
            isWrite = true;
        }

        if (strcmp(argv[i], "--ram") == 0)
        {
            isRam = true;
        }

        if (strcmp(argv[i], "--rom") == 0)
        {
            isRom = true;
        }

        if (strcmp(argv[i], "-v") == 0)
        {
            std::stringstream temp;
            temp << argv[i + 1];
            temp >> value;

            if (value == 0)
            {
                temp.str("");
                temp << std::hex << argv[i + 1];
                temp >> value;
            }
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            path = argv[i + 1];
            isFile = true;
        }

        if (strcmp(argv[i], "-fo") == 0)
        {
            std::stringstream temp;
            temp << argv[i + 1];
            temp >> fileOffset;

            if (fileOffset == 0)
            {
                temp.str("");
                temp << std::hex << argv[i + 1];
                temp >> fileOffset;
            }
        }

        
    }

    if (isRam && isRom && isWrite)
    {
        throw "Only one type of memory is permitted.";
    }

    if ((!isRam && !isRom) && isWrite)
    {
        throw "You need to give a type of memory.";
    }

    if (!hasBytes && isWrite)
    {
        bytes = 64;
    }

    return { offset, bytes, isWrite, isFile, path, fileOffset, isRam, isRom, value };
}
