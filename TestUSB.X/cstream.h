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
    
    typedef void* STREAM;
    typedef void* HANDLE;
    typedef unsigned char TYPE;
#define USB_STREAM (char)0x45
#define LOG_STREAM (char)0x123

    extern void WriteStream(STREAM stream, size_t offset, size_t bytes);
    extern void ReadStream(STREAM stream, size_t offset, size_t bytes);
    extern void CopyTo(STREAM streamSrc, STREAM streamDest, size_t readOffset, size_t writeOffset);
    extern int ProgramMemToStream(const char* src, TYPE type, size_t offset, int bytes);

#ifdef	__cplusplus
}
#endif

#endif	/* CSTREAM_H */
#endif
