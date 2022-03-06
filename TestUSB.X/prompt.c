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
//    if (command == NULL) 
//    {
//        command = CreateList(size);
//    }
//    else 
//    {
//        kclearAll(command);
//    }
//
//    processCommands(arg);
}

//void closeCommand() 
//{
////    kfree(command);
//}

//void* getCommand() 
//{
////    return command;
//}

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
//    {
//        memset(result, 0, RESULT_LENGTH);
//        foreach(command) 
//        {
//            char c = kgetchar(command);
//
//            if (c == '-' && step == 0) 
//            {
//                step++;
//            }
//            else if (step == 1 && c == cmd)
//            {
//                step++;
//                break;
//            }
//            else {
//                step = 0;
//            }
//        }
//        krewind(command);
//    }
//
//    return step == 2;
}

char* getSubCommandValue(char cmd) 
{
    int step = 0;
    memset(result, 0, RESULT_LENGTH);

    for (int i = 0; i < _size; i++) 
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

            result[i] = c;
        }
        else {
            step = 0;
        }
    }

    return result;
//    {
//        int step = 0;
//        int j = 0;
//        memset(result, 0, RESULT_LENGTH);
//        foreach(command) 
//        {
//            char c = kgetchar(command);
//
//            if ((c == '-') && step > 0) 
//            {
//                break;
//            }
//            else if ((c == '-') || (step == 1 && c == cmd) || (step == 2 && c == ' ')) 
//            {
//                step++;
//            }
//            else if (step == 3) 
//            {
//                if (c == ' ' || c == '\0') 
//                {
//                    break;
//                }
//
//                result[j] = c;
//                j++;
//            }
//            else {
//                step = 0;
//            }
//        }
//        krewind(command);
//    }
//    return result;
}

//void processCommands(char* arg) 
//{
////    size_t len = strlen(arg);
//////    PORTB = (char)len;
////    for(int i = 0; i < len; i++, arg++) 
////    {
////        kpush(command, (*arg));
////    }
//}

#endif
