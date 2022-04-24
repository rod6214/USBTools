#include <xc.h>
#include "kernel.h"
#include "logger.h"
#include "cstream.h"

#if __LOGGER__
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

volatile Stream_t stream;

void* CreateLogger(int bytes)
{
    if (stream.ptr == NULL) 
    {
        stream.ptr = CreateList(bytes);
        stream.index = 0;
        stream.type = LOG_STREAM;
    }

    return (void*)&stream;
}

void log_rewind() 
{
    krewind(stream.ptr);
}

void* log_getStream() 
{
    stream.index = 0;
    stream.type = LOG_STREAM;
    return (void*)&stream;
}

int log_putchar(char c) 
{
    char result = kpush(stream.ptr, c);
    return result;
}

char log_getchar() 
{
    char c = kgetchar(stream.ptr);
    knext(stream.ptr);
    return c;
}

void log_free() 
{
//    kfree(stream);
    kfree(stream.ptr);
    stream.ptr = NULL;
//    stream = NULL;
}

#endif
