#if __LOGGER_TEST__
#include "logger_test.h"
#include <string.h>
#include <xc.h>

#define LOGGER_BUFFER_LENGTH 64
const char* logger_text = "Program loaded with pack,PIC18Fxxxx_DFP,1.3.36,Microchip";
char logger_buffer[LOGGER_BUFFER_LENGTH];

int _logger_setMemory()
{
    size_t len = strlen(logger_text);
    memset(logger_buffer, 0, LOGGER_BUFFER_LENGTH);
    strncpy(logger_buffer, logger_text, len);
    return (int)len;
}

int Logger_CanWriteToTheStream()
{
   int size = _logger_setMemory();
   Stream_t* stream = CreateLogger(size + 1);
   int assert = TRUE;
   
   WriteStream(stream, logger_buffer, 0, (size_t)size);
   
   {
       foreach(stream->ptr) 
       {
           char c = kgetchar(stream->ptr);
           if (c != logger_buffer[i]) 
           {
               assert = FALSE;
               break;
           }
       }
       krewind(stream->ptr);
   }
   
   return assert;
}

int Logger_CanReadToTheStream()
{
   int size = _logger_setMemory();
   Stream_t* stream = CreateLogger(size + 1);
   int assert = TRUE;
   WriteStream(stream, logger_buffer, 0, (size_t)size);
   ReadStream(stream, logger_buffer, 0);
   assert = strncmp(logger_buffer, logger_text, size) == 0;
   return assert;
}

int Logger_ExecuteTests() 
{
    int fails = 0;

    if (!Logger_CanWriteToTheStream()) 
    {
        fails++;
    }
    
    if (!Logger_CanReadToTheStream()) 
    {
        fails++;
    }
    return fails;
}
#endif
