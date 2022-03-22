#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "prompt.h"
#include "kernel.h"

#if __DEVTOOL__


enum _States {
    COMMAND,
    SUBCOMMAND
} States;

void* command = NULL;
char result[RESULT_LENGTH];
char* _buffer = 0;
int _size = 0;

void showVersion() {}

void commandLine(char *arg, int size) 
{
    _buffer = arg;
    _size = size;
}

char* getCommandKey() 
{
   memset(result, 0, RESULT_LENGTH);

   for (int i = 0; i < _size; i++) 
   {
        char c = _buffer[i];
        if (c == '\0' || c == ' ') 
        {
            break;
        }
        result[i] = c;
   }

   return result;
}

int subCommandExists(char cmd) 
{
   int step = 0;
   memset(result, 0, RESULT_LENGTH);

   for (int i = 0; i < _size; i++) 
   {
       char c = _buffer[i];
        if (c == '-' && step == 0) 
        {
            step++;
        }
        else if (step == 1 && c == cmd)
        {
            step++;
            break;
        }
        else {
            step = 0;
        }
   }

   return step == 2;
}

char* getSubCommandValue(char cmd) 
{
    int step = 0;
    memset(result, 0, RESULT_LENGTH);

    for (int i = 0, j = 0; i < _size; i++) 
    {
        char c = _buffer[i];

        if ((c == '-') && step > 0) 
        {
            break;
        }
        else if ((c == '-') || (step == 1 && c == cmd) || (step == 2 && c == ' ')) 
        {
            step++;
        }
        else if (step == 3) 
        {
            if (c == ' ' || c == '\0') 
            {
                break;
            }

            result[j++] = c;
        }
        else {
            step = 0;
        }
    }

    return result;
}

#endif
