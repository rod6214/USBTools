#include <stdlib.h>
#include "cstream.h"
#include "usb.h"
#include "logger.h"

#if __STREAM__

typedef struct _Stream 
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
} _Stream_t;

static void _copymem(HANDLE src, HANDLE dest, size_t offset, size_t bytes);
static int _putchar(char c, TYPE type);
static char _getchar(TYPE type);
static int _bytesReadOrWrite(int i, size_t offset);


int ProgramMemToStream(const char* src, TYPE type, size_t offset, int bytes) 
{
    char data;
    int i = 0;

    for (i = offset; i < bytes; i++) 
    {
        data = src[i];
        _putchar(data, type);
    }

    int total = _bytesReadOrWrite(i, offset);
    return total;
}

static int _bytesReadOrWrite(int i, size_t offset) 
{
    int sigOff = (int)offset;
    int len = (int)(i + 1 - sigOff);
    int result = abs(len);
    return result;
}

void WriteStream(STREAM stream, size_t offset, size_t bytes) 
{
    _Stream_t* strm = (_Stream_t*)stream;
    TYPE type = strm->type;
    unsigned char* elements = (unsigned char*)strm->writeHandle;
    char data;
    int i = 0;

    for(i = (int)offset; i < bytes; i++) 
    {
        data = elements[i];
        _putchar(data, type);
    }

    strm->bytesWrite = _bytesReadOrWrite(i, offset);
    // int sigOff = (int)offset;
    // int len = (int)(i + 1 - sigOff);
    // strm->bytesWrite = abs(len);
}

void ReadStream(STREAM stream, size_t offset, size_t bytes) 
{
    _Stream_t* strm = (_Stream_t*)stream;
    TYPE type = strm->type;
    unsigned char* elements = (unsigned char*)strm->readHandle;
    char data = '-';
    int i = 0;
    
    while (!data)
    {
        data = _getchar(type);
        (elements)[i++] = data;
    }
    
    strm->bytesRead = _bytesReadOrWrite(i, offset);
    // int sigOff = (int)offset;
    // int len = (int)(i + 1 - sigOff);
    // strm->bytesRead = abs(len);
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

void CopyTo(STREAM streamSrc, STREAM streamDest, size_t readOffset, size_t writeOffset) 
{
    if (streamSrc != NULL && streamDest != NULL) 
    { 
        _Stream_t* strmSrc = (_Stream_t*)streamSrc;
        _Stream_t* strmDest = (_Stream_t*)streamDest;
        
        _copymem(strmSrc->readHandle, strmDest->readHandle, readOffset, strmSrc->readBufferSize);
        _copymem(strmSrc->writeHandle, strmDest->writeHandle, writeOffset, strmSrc->writeBufferSize);
    }
}
#endif