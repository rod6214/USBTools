#ifndef USBPIC_H
#define USBPIC_H
#define ENDPOINT_0_SIZE 8

// Buffer Descriptor bit masks (from PIC datasheet)
#define UOWN   0x80 // USB Own Bit
#define DTS    0x40 // Data Toggle Synchronization Bit
#define KEN    0x20 // BD Keep Enable Bit
#define INCDIS 0x10 // Address Increment Disable Bit
#define DTSEN  0x08 // Data Toggle Synchronization Enable Bit
#define BSTALL 0x04 // Buffer Stall Enable Bit
#define BC9    0x02 // Byte count bit 9
#define BC8    0x01 // Byte count bit 8


typedef unsigned char BYTE;

typedef struct _BDT
{
    unsigned char STAT;
    unsigned char CNT;
    unsigned int ADDR;
} BDT; //Buffer Descriptor Table

// Every device request starts with an 8 unsigned char setup packet (USB 2.0, chap 9.3)
// with a standard layout.  The meaning of wValue and wIndex will
// vary depending on the request type and specific request.
typedef struct _setup_packet_struct
{
    unsigned char bmrequesttype; // D7: Direction, D6..5: Type, D4..0: Recipient
    unsigned char brequest;      // Specific request
    unsigned char wvalue0;       // LSB of wValue
    unsigned char wvalue1;       // MSB of wValue
    unsigned char windex0;       // LSB of wIndex
    unsigned char windex1;       // MSB of wIndex
    unsigned short wlength;      // Number of unsigned chars to transfer if there's a data stage
    void* extra;
//    unsigned char extra[56];     // Fill out to same size as Endpoint 0 max buffer
} setup_packet_struct;

#define USTAT_IN (0x04)
#define USTAT_OUT (0x00)

//endpoints
extern volatile BDT ep0_o __at (0x0400+0*8);
extern volatile BDT ep0_i __at (0x0404+0*8);
extern volatile BDT ep1_o __at (0x0400+1*8);
extern volatile BDT ep1_i __at (0x0404+1*8);
extern volatile BDT ep2_o __at (0x0400+2*8);
extern volatile BDT ep2_i __at (0x0404+2*8);
extern volatile BDT ep3_o __at (0x0400+3*8);
extern volatile BDT ep3_i __at (0x0404+3*8);

#endif
