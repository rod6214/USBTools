#include <xc.h>
#include "kernel.h"
#include "logger.h"
#include "cstream.h"

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

typedef struct _Allocation 
{
  void* next;
  void* ptr;
  void* link;
  int used;
  int length;
  int pos;
  int index;
} Allocation_t;

Allocation_t* logList = NULL;
_PrinterStream_t* stream;

void* CreateLogger(int bytes)
{
    if (stream == NULL) 
    {
        stream = kmalloc(sizeof(_PrinterStream_t));
        logList = CreateList(bytes);
        stream->readBufferSize = (size_t)bytes;
        stream->writeBufferSize = (size_t)bytes;
    }

    return logList;
}

void log_rewind() 
{
    krewind(&logList);
}

void* log_getStream() 
{
    stream->writeHandle = logList->ptr;
    stream->readHandle = logList->ptr;
    stream->index = 0;
    stream->type = LOG_STREAM;
    return stream;
}

int log_putchar(char c) 
{
    char result = kpush(&logList, c);
    return result;
}

char log_getchar() 
{
    char c = kgetchar(logList);
    logList = knext(logList);
    return c;
}

void log_free() 
{
    kfree(stream);
    kfree(logList);
    logList = NULL;
    stream = NULL;
}
