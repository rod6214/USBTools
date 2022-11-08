#include "global.h"
#include "descriptors.h"


volatile BD_t BD0_out __at(0x400);
volatile BD_t BD0_in __at(0x404);
volatile BD_t BD1_out __at(0x408);
volatile BD_t BD1_in __at(0x40C);
volatile TokenPacket_t requestPacket __at(0x500);
uint8_t buffer_in[USB_EP_BUFFER_LEN] __at(0x508);
uint8_t ep1_tx_buffer[USB_EP_BUFFER_LEN] __at(0x548);
uint8_t ep1_rx_buffer[USB_EP_BUFFER_LEN] __at(0x588);
size_t descriptorLength = 0;
int usb_status = DEFAULT;
int req_handled = FALSE;
size_t req_len = 0;
uint16_t address = 0;
int st = 0;
//dataPtr* ptrData;
codePtr *pDescriptor;
int usb_device_status = DETACHED;
BYTE current_configuration = 0;  
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
        PKTDIS = 0;
        USB_prepare_ep_control();
        TRNIE = 1;
        URSTIF = 0;
        usb_device_status = DEFAULT;
        current_configuration = 0; 
    }
    
    if (UIRbits.SOFIF && UIEbits.SOFIE) {
		UIRbits.SOFIF = 0;
	}
    
    // stall processing
	if (UIRbits.STALLIF && UIEbits.STALLIE) {
		if (UEP0bits.EPSTALL == 1) {
			// Prepare for the Setup stage of a control transfer
			USB_prepare_ep_control();
			UEP0bits.EPSTALL = 0;
		}
		UIRbits.STALLIF = 0;
	}
	if (UIRbits.UERRIF && UIEbits.UERRIE) {
		// Clear errors
		UIRbits.UERRIF = 0;
	}
    
    if (TRNIE && TRNIF) 
    {
        USB_process_control_transfer();
        while(TRNIF == 1)
            TRNIF = 0;
    }
    ei();
}

void usb_read(int ep_addr) {
    switch(ep_addr) 
    {
        case 1:
        {
            FLUSH(BD1_out);
        }
        break;
        default:
            break;
    }
}

void load_in_data() 
{
    size_t len;
    if (req_len < USB_BUFFER_CONTROL_SIZE)
        len = req_len;
    else
        len = USB_BUFFER_CONTROL_SIZE;
    
    dataPtr *ptrData = (dataPtr*)&buffer_in[0];
    
    BD0_in.BDSTAT &= ~(BC8| BC9);
    BD0_in.BDCNT = len & 0xff;
    BD0_in.ADDR = (uint16_t)ptrData;
    
    req_len = req_len - len;
    
    for (int i = 0; i < len; i++) 
    {
        *(ptrData++) = *(pDescriptor++);
    }
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
    usb_device_status = ADDRESS;
    address = requestPacket.wValue;
    req_handled = TRUE;
}

void Get_Descriptor()
{
    if (!(requestPacket.bmRequestType & 0x80)) 
        return;

    uint8_t descriptorType = (requestPacket.wValue >> 8) & 0xff;
    uint8_t index = (requestPacket.wValue) & 0xff;
    
    switch(descriptorType) 
    {
        case DEVICE:
        {
            pDescriptor = (void*)&device_descriptor;
            req_handled = TRUE;
            req_len = *((uint8_t*)pDescriptor);
        }
        break;
        case CONFIGURATION:
        {
            pDescriptor = (void*)&config_descriptor;
            req_handled = TRUE;
            req_len = config_descriptor.configDesc.wTotalLength;
        }
        break;
        case STRING:
        {
            pDescriptor = (codePtr*)string_descriptors[index];
            req_handled = TRUE;
            req_len = string_descriptors[index][0] & 0xff;
        }
        break;
        default:
            pDescriptor = 0;
            break;
    }
}

void set_configuration() 
{
    req_handled = 1;
    current_configuration = requestPacket.wValue & 0xff;
    // TBD: ensure the new configuration value is one that
    // exists in the descriptor.
    if (current_configuration == 0) {
        // If configuration value is zero, device is put in
        // address state (USB 2.0 - 9.4.7)
        usb_device_status = ADDRESS;
    } else {
        // Set the configuration.
        usb_device_status = CONFIGURED;
        {
            UEP1 = 0x1E;
            BD0_out.BDCNT = USB_EP_BUFFER_LEN;
            BD0_out.ADDR = (uint16_t) ep1_rx_buffer;
            BD0_out.BDSTAT = UOWN | DTSEN;
            BD0_in.ADDR = (uint16_t) ep1_tx_buffer;
            BD0_in.BDSTAT = DTS;
        }
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
            req_handled = FALSE;
            pDescriptor = 0;
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
                    set_configuration();
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
                    if (requestPacket.wLength < req_len)//9.4.3, p.253
                        req_len = requestPacket.wLength;
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
                BD0_out.BDCNT = USB_BUFFER_CONTROL_SIZE;
				BD0_out.ADDR = (uint16_t) &requestPacket;
				BD0_out.BDSTAT = UOWN | BSTALL;
				BD0_in.BDSTAT = UOWN | BSTALL;
            }
            
            PKTDIS = 0;
        }
        else if (usb_status == DATA_OUT_STAGE) 
        {
            unsigned char bufferSize;
				bufferSize = BD0_out.BDCNT;
                
				// Accumulate total number of unsigned chars read
				req_len = req_len + bufferSize;
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
        if ((UADDR == 0) && (usb_device_status == ADDRESS))
        {
            UADDR = (uint8_t)address; 
            if (UADDR == 0) {
				usb_device_status = DEFAULT;
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
    else if (IS_OUT_EP1) {
        usb_read(1);
        PORTB++;
    }
//    else if (IS_IN_EP1) 
//    {
////        usb_read(1);
//        PORTB=6;
//    }
    
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
    usb_device_status = ATTACHED;
    ei();
}