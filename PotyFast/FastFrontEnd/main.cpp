#include "Generic.h"
#include <streambuf>

typedef struct _CONFIG_STEP
{
    std::unique_ptr<CONNECT::ThreadStatus>& sts;
    UTILS::Cstring path;
}MainConfig_t;

MainConfig_t usb_config();

int main()
{
    auto config = usb_config();
    auto usb_conf = std::unique_ptr<CONNECT::USBConfig>(new CONNECT::USBConfig(config.sts, config.path));
    Z80_CONNECT::Z80Connector z80card(usb_conf);

    try {
        /*z80card.Reset();
        z80card.OneStep();*/
        //const char* request = "FROM VISUAL STUDIO";
        //char request[4] = { 1, 2, 4, 8};
        //auto  value = z80card.WriteMemory(request, 0, 3);
        //auto value = z80card.ReadMemory(0, 3);
        z80card.OneStep();
        z80card.ProgramMode();
        z80card.Reset();
        z80card.Run();
        //std::cout << value.bytes << std::endl;
        /*auto total = z80card.SendCommand(0xaa, "Test USB pepepep!!!", 48);
        std::cout << total << std::endl;
        unsigned char* buffer = new unsigned char[64];
        total = z80card.GetResponse(buffer, 48);
        std::cout << total << std::endl;*/
    }
    catch (const char* err) {
        std::cout << err << std::endl;
    }

    while (1) {}
	return 0;
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