#include <stdlib.h>
#include "cstream.h"
#include "usb.h"

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

void WriteStream(STREAM stream, size_t offset, size_t bytes) 
{
    _Stream_t* strm = (_Stream_t*)stream;
    TYPE type = strm->type;
    unsigned char* elements = (unsigned char*)strm->writeHandle;
    char data;
    size_t i = 0;

    for(i = offset; i < bytes; i++) 
    {
        data = elements[i];
        _putchar(data, type);
    }

    strm->bytesWrite = abs(i + 1 - offset);
}

void ReadStream(STREAM stream, size_t offset, size_t bytes) 
{
    _Stream_t* strm = (_Stream_t*)stream;
    TYPE type = strm->type;
    unsigned char* elements = (unsigned char*)strm->readHandle;
    char data = 'a';
    int i = 0;
    
    while (!data)
    {
        data = _getchar(type);
        (elements)[i++] = data;
    }
    
    strm->bytesRead = abs(i + 1 - offset);
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
    }
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
            break;
    }
}

static void _copymem(HANDLE src, HANDLE dest, size_t offset, size_t bytes) 
{
    if (src!= NULL && dest != NULL) 
    {    
        for(size_t i = 0; i < bytes; i++) 
        {
            (dest)[i] = (src)[i];
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
