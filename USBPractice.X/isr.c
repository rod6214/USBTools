#include "global.h"
#include "descriptors.h"


volatile BD_t BD0_out __at(0x400);
volatile BD_t BD0_in __at(0x404);
volatile TokenPacket_t requestPacket __at(0x500);
uint8_t buffer_in[USB_EP_BUFFER_LEN] __at(0x508);
size_t descriptorLength = 0;
int usb_status = DETACHED;
int req_handled = FALSE;
size_t req_len = 0;
uint16_t address = 0;
int st = 0;
dataPtr* ptrData;
codePtr *pDescriptor;

void USB_prepare_ep_control(void);
void USB_process_control_transfer(void);


void __interrupt(high_priority) high_isr() 
{
    di();
    // Clear USB interrupt flag
    USBIF = 0;
    if (URSTIE && URSTIF) 
    {
        while(TRNIF == 1)
            TRNIF = 0;
        UIR = 0;
        
        USB_prepare_ep_control();
        TRNIE = 1;
        URSTIF = 0;
        usb_status = DEFAULT;
    }
    
    if (TRNIE && TRNIF) 
    {
        USB_process_control_transfer();
        while(TRNIF == 1)
            TRNIF = 0;
    }
    ei();
}

uint16_t offset = 0;

void load_in_data() 
{
    size_t len;
//    if (req_len < (requestPacket.wLength & 0xff))
//        req_len = (requestPacket.wLength & 0xff);
    if (req_len > USB_BUFFER_CONTROL_SIZE)
        len = USB_BUFFER_CONTROL_SIZE;
    else
        len = req_len;
    
    codePtr* pSrc = pDescriptor;
    
    for (int i = 0; i < offset; i++)
        pSrc++;
    
    BD0_in.BDSTAT &= ~(BC8| BC9);
//    BD0_in.BDCNT = 18;
    BD0_in.BDCNT = len & 0xff;
    BD0_in.ADDR = (uint16_t)ptrData;
    
    for (int i = 0; i < len; i++) 
    {
        *(ptrData++) = *(pSrc++);
    }
    
    req_len -= len;
    offset += len;
}

void USB_prepare_ep_control() 
{
    UEP0 = 0;
    EP0INEN = 1;
    EP0OUTEN = 1;
    EP0HSHK = 1;
    BD0_out.BDCNT = USB_BUFFER_CONTROL_SIZE;
    BD0_out.ADDR = (uint16_t)&requestPacket;
    BD0_out.BDSTAT = UOWN | DTSEN;
    BD0_in.BDSTAT = 0;
    PKTDIS = 0;
}

void Set_Address() 
{
    usb_status = ADDRESS;
    address = requestPacket.wValue;
    req_handled = TRUE;
}

void Get_Descriptor()
{
    if (!(requestPacket.bmRequestType & 0x80)) 
        return;
    ptrData = (dataPtr*)buffer_in;
    offset = 0;
    uint8_t descriptorType = (requestPacket.wValue >> 8) & 0xff;
    uint8_t index = (requestPacket.wValue) & 0xff;
    
//    if (st == 0) 
//        {
//            PORTB = (requestPacket.wLength) & 0xff;
//        }
//        
//        if (st < 10)st++;
    
    switch(descriptorType) 
    {
        case DEVICE:
        {
//            PORTB = (requestPacket.wLength) & 0xff;;
            pDescriptor = (void*)&device_descriptor;
            req_handled = TRUE;
            
            req_len = *((uint8_t*)pDescriptor);
        }
        break;
        case CONFIGURATION:
        {
            PORTB = 2;
            pDescriptor = (void*)&config_descriptor;
            req_handled = TRUE;
            req_len = config_descriptor.configDesc.wTotalLength;
        }
        break;
        case STRING:
        {
            PORTB = 3;
            pDescriptor = (codePtr*)string_descriptors[index];
            req_handled = TRUE;
            req_len = string_descriptors[index][0] & 0xff;
        }
        break;
        default:
            
            break;
    }
}

void USB_process_control_transfer() 
{
    if (IS_OUT_EP0) // Control transfer 
    {
        
        
        BD0_out.BDSTAT &= ~UOWN;
		BD0_in.BDSTAT &= ~UOWN;
        if (IS_SETUP(BD0_out)) 
        {
            switch(requestPacket.bRequest) 
            {
                case SET_ADDRESS: 
                {
                    Set_Address();
                }
                break;
                case GET_DESCRIPTOR:
                {
                    Get_Descriptor();
                }
                break;
                case GET_CONFIGURATION:
                {
                    PORTB = 8;
                }
                break;
                case SET_CONFIGURATION:
                {
                    PORTB = 9;
                }
                break;
                case SET_INTERFACE: 
                {
                    PORTB=5;
                }
                break;
                default:
                    
                    break;
            }
            
            if (req_handled) 
            {
                if (requestPacket.bmRequestType & 0x80) 
                {
                    // Process Device > Host
                    usb_status = DATA_IN_STAGE;
                    load_in_data();
                    BD0_out.BDCNT = USB_BUFFER_CONTROL_SIZE;
                    BD0_out.ADDR = (uint16_t)&requestPacket;
                    BD0_out.BDSTAT = UOWN;
                    BD0_in.BDSTAT = UOWN | DTSEN | DTS;
                }
                else 
                {
                    // Process Host > Device
                    usb_status = DATA_OUT_STAGE;
                    BD0_in.BDCNT = 0;
                    BD0_in.BDSTAT = UOWN | DTSEN | DTS;
                    BD0_out.BDCNT = USB_BUFFER_CONTROL_SIZE;
                    BD0_out.BDSTAT = UOWN | DTSEN | DTS;   
                }
            }
            else 
            {
                PORTB = 1;
            }
            
            PKTDIS = 0;
        }
        else if (usb_status == DATA_OUT_STAGE) 
        {
            
            if (BD0_out.BDSTAT & DTS)
                BD0_out.BDSTAT = UOWN | DTSEN;
            else
                BD0_out.BDSTAT = UOWN | DTSEN | DTS;
        }
        else 
        {
            USB_prepare_ep_control();
        }
    }
    else if (IS_IN_EP0) // Control transfer
    {
        if (usb_status == ADDRESS) 
        {
            UADDR = (uint8_t)address; 
            if (UADDR == 0) {
				usb_status = DEFAULT;
			}
        }
        
        if (usb_status == DATA_IN_STAGE) 
        {
            load_in_data();
            if (BD0_in.BDSTAT & DTS)
                BD0_in.BDSTAT = UOWN | DTSEN;
            else
                BD0_in.BDSTAT = UOWN | DTSEN | DTS;
        }
        else 
        {
            USB_prepare_ep_control();
        }
    }
}

void USB_init() 
{
    di();
    // Clear registers
    UCFG = 0;
    UCON = 0;
    UIR = 0;
    UIE = 0;
    // Enable priority interrupt selection
    IPEN = 1;
    // Configure USB as high priority
    USBIP = 1;
    // Activate on-chip pull-up
    FSEN = 1;
    // Activate full speed
    UPUEN = 1;
    // Reset interrupt enable
    URSTIE = 1;
    // Activate USB interrupt
    USBIE = 1;
    // Attached USB port
    USBEN = 1;
    usb_status = ATTACHED;
    ei();
}