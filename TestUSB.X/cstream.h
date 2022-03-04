/* 
 * File:   cstream.h
 * Author: Nelson
 *
 * Created on February 21, 2022, 8:57 PM
 */
#if __STREAM__
#ifndef CSTREAM_H
#define	CSTREAM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
    
#define USB_STREAM (char)0x45
#define LOG_STREAM (char)0x123
#define MEMORY_STREAM (char)0x28

typedef void* STREAM;
typedef void* HANDLE;
typedef unsigned char TYPE;
    
typedef struct _Stream 
{
    HANDLE ptr;
    unsigned int index;
    TYPE type;
    unsigned char sector;
} Stream_t;

extern int WriteStream(STREAM stream, char* __restrict ptr, size_t offset, size_t bytes);
extern int ReadStream(STREAM stream, char* __restrict ptr, size_t offset);
extern void CloseMemoryStream();
extern void ResetStreamMemory();

#ifdef	__cplusplus
}
#endif

#endif	/* CSTREAM_H */
#endif
