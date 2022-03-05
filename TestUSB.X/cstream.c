#include <stdlib.h>
#include "cstream.h"
#include "kernel.h"
#include "usb.h"
#include "logger.h"

#if __STREAM__

static void _copymem(HANDLE src, HANDLE dest, size_t offset, size_t bytes);
static int _putchar(char c, TYPE type);
static char _getchar(TYPE type);
static int _bytesReadOrWrite(int i, size_t offset);

static int _bytesReadOrWrite(int i, size_t offset) 
{
    int sigOff = (int)offset;
    int len = (int)(i + 1 - sigOff);
    int result = abs(len);
    return result;
}

void ResetStreamMemory() 
{
    log_rewind();
}

void CloseMemoryStream() 
{
    log_free();
}

int WriteStream(STREAM stream, char* __restrict ptr, size_t offset, size_t bytes) 
{
    Stream_t* strm = (Stream_t*)stream;
    TYPE type = strm->type;
    char data;
    int i = 0;

    for(i = (int)offset; i < bytes; i++) 
    {
        data = ptr[i];
        _putchar(data, type);
    }

    int _bytes = _bytesReadOrWrite(i, offset);
    return _bytes;
}

int ReadStream(STREAM stream, char* __restrict ptr, size_t offset) 
{
    Stream_t* strm = (Stream_t*)stream;
    TYPE type = strm->type;
    char data = '\0';
    int i = 0;
    
    do 
    {
        data = _getchar(type);
        (ptr)[(int)offset + i] = data;
        i++;
    } 
    while(data != '\0');
    
    int _bytes = _bytesReadOrWrite(i, offset);
    return _bytes;
}

static int _putchar(char c, TYPE type) 
{
    switch(type) 
    {
        case USB_STREAM:
        {
            int bytes = usb_putchar(c);
            return bytes;
        }
        case LOG_STREAM:
        {
            int bytes = log_putchar(c);
            return bytes;
        }
    }
    
    return 0;
}

static char _getchar(TYPE type) 
{
    switch(type) 
    {
        case USB_STREAM:
        {
            char c = usb_getchar();
            return c;
        }
        case LOG_STREAM:
        {
            char c = log_getchar();
            return c;
        }
    }
    return 0;
}

static void _copymem(HANDLE src, HANDLE dest, size_t offset, size_t bytes) 
{
    if (src!= NULL && dest != NULL) 
    {    
        for(size_t i = offset; i < bytes; i++) 
        {
            char* _dest = (char*)dest;
            char* _src = (char*)src;
            (_dest)[i] = (_src)[i];
        }
    }
}

#endif