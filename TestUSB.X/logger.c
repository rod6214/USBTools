#include <xc.h>
#include "logger.h"
// Commands
#define PWM_MODULE 0x22
#define MEMORY 0x52
#define VERSION 0x44

// Functions
// PWM_MODULE
#define SET_PWM_CCP_REGISTER 0x33
#define GET_PWM_CCP_REGISTER 0x32
// MEMORY
#define SET_MEMORY 0x12
#define GET_MEMORY 0x13

typedef struct _PrinterStream 
{
    HANDLE writeHandle;
    HANDLE readHandle;
    unsigned int index;
    size_t readBufferSize;
    size_t writeBufferSize;
    int bytesRead;
    int bytesWrite;
    TYPE type;
    unsigned char sector;
} _PrinterStream_t;

void CreateLogger(STREAM stream) {}
//void ExcuteCommand(STREAM stream) {}
//void ReadCommand(STREAM stream) {}
