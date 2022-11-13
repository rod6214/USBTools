#include <iostream>
#include "usb.h"

#define GET_STATUS         0
#define CLEAR_FEATURE      1
#define SET_FEATURE        3
#define SET_ADDRESS        5
#define GET_DESCRIPTOR     6
#define SET_DESCRIPTOR     7
#define GET_CONFIGURATION  8
#define SET_CONFIGURATION  9
#define GET_INTERFACE     10
#define SET_INTERFACE     11
#define SYNCH_FRAME       12

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
    auto usb = std::unique_ptr<CONNECT::USB>(new CONNECT::USB(usb_conf));

    if (usb->USB_Init()) 
    {
        std::cout << "USB initialized" << std::endl;
    }
    
    usb->~USB();
    usb.release();
    while(1){}
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
