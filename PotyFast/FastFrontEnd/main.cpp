#include "Generic.h"

typedef struct _CONFIG_STEP
{
    std::unique_ptr<CONNECT::ThreadStatus>& sts;
    UTILS::Cstring path;
}MainConfig_t;

MainConfig_t usb_config();

int main()
{
	//test();
    auto config = usb_config();
    auto usb_conf = std::unique_ptr<CONNECT::USBConfig>(new CONNECT::USBConfig(config.sts, config.path));
    auto usb = std::unique_ptr<CONNECT::USB>(new CONNECT::USB(usb_conf));

    if (usb->USB_Init())
    {
        std::cout << "USB initialized" << std::endl;
    }
    else 
    {
        std::cout << "USB error" << std::endl;
    }

    usb->~USB();
    usb.release();
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