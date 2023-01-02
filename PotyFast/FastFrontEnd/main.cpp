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
} MainArgs_t;

MainConfig_t usb_config();
MainArgs_t process_args(int args, const char* argv[]);

int main(int args, const char* argv[])
{   
    auto mainArgs = process_args(args, argv);

    if ((mainArgs.offset > 65535) || (mainArgs.bytes > 65536))
    {
        throw "Offset is out of range.";
    }

    if ((mainArgs.offset + mainArgs.bytes) > 65535) 
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

            if (mainArgs.isWrite)
            {       
                if ((mainArgs.bytes % 64) != 0)
                {
                    throw "Number of bytes should be multiple of 64.";
                }

                if ((mainArgs.offset % 64) != 0)
                {
                    throw "Offset should be multiple of 64 or 0.";
                }

                std::fstream file;
                file.open(mainArgs.path, std::ios::binary | std::ios::in);
                
                char* binary = new char[mainArgs.bytes];
                char* ptr = binary;
                int val = 0, len = 0;

                while (!file.eof() && (len < mainArgs.bytes))
                {                    
                    *(ptr++) = 0xff & file.get();
                    len++;
                }

                file.close();

                z80card.WriteMemory(binary, mainArgs.offset, mainArgs.bytes);

                delete[] binary;
            }
            else
            {
                auto value = z80card.ReadMemory(mainArgs.offset, mainArgs.bytes);
                int row = 0;
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

            std::cout << data_mesh.str() << std::endl;
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
    int bytes = 0, offset = 0;
    bool isWrite = false, isFile = false;
    const char* path = "";

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
        else if (strcmp(argv[i], "-b") == 0)
        {
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
        else if (strcmp(argv[i], "-w") == 0) 
        {
            isWrite = true;
        }
        else if (strcmp(argv[i], "-f") == 0) 
        {
            isFile = true;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            path = argv[i + 1];
        }
    }
    return { offset, bytes, isWrite, isFile, path };
}
