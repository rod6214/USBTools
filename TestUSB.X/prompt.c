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
void* subCommands = NULL;
char result[16];

void showVersion() {}

void commandLine(char *arg) 
{
    if (command == NULL || subCommands == NULL) 
    {
        command = CreateList(8);
        subCommands = CreateList(64);
    }
    
    processCommands(arg, command, subCommands);
}

void closeCommand() 
{
    kfree(command);
    kfree(subCommands);
}

void* getCommand() 
{
    return command;
}

void* getSubcommands() 
{
    return subCommands;
}

char* getCommandKey() 
{
    ktoArray(command, result);
    return result;
}

char getSubCommandKey(int index) 
{
    {
        int j = 0;
        int closeTokens = 0;
        foreach(subCommands)
        {
            char c = kgetchar(subCommands);
            
            if ((c == '-' && closeTokens == 0)) 
            {
                closeTokens++;
            }
            else if (closeTokens == 1 && j == index) 
            {
                return c;
            }
            
            j++;
        }
    }
    
    return (char)-1;
}

int subCommandExists(char c) 
{
    char r;
    int index = 0;

    while(r != (char)-1) 
    {
        r = getSubCommandKey(index);
        index++;
    }

    return r != (char)-1;
}

char* getSubCommandValue(char command) 
{
    {
        int j = 0;
        int closeTokens = 0;
        memset(result, 0, 16);
        foreach(subCommands)
        {
            char c = kgetchar(subCommands);
            
            if ((c == '-' && closeTokens == 0) || (closeTokens == 1 && c == command)) 
            {
                closeTokens++;
            }
            else if (closeTokens == 2 && c == '\0') 
            {
                return result;
            }
            else if (closeTokens == 2) 
            {
                result[j] = c;
                j++;
            }
        }
    }
    return NULL;
}

void processCommands(char* arg, void* command, void* subCommands) 
{
    char temp[16];
    int index = 0;
    int state = COMMAND;
    
    while((*arg) != '\0')
    {
        char value = *(arg++);
        temp[index] = value;
        if (value == ' ' && state == COMMAND) 
        {
            temp[index] = '\0';
            kaddRange(command, temp, 0, index + 1);
            state = SUBCOMMAND;
        }
        
        if (value == '-' && state == SUBCOMMAND) 
        {
            kpush(subCommands, value);
        }
        else if (value != ' ' && state == SUBCOMMAND) 
        {
            // int j = 0;
            int finish = FALSE;
            int closeTokens = 0;
            int initToken = 0;
            
            while(!finish) 
            {
                if (value == '-')
                {
                    initToken++;
                    arg--;
                }
                
                if (initToken) 
                {
                    finish = TRUE;
                    kpush(subCommands, '\0');
                    continue;
                }
                
                if (value != ' ') 
                {
                    kpush(subCommands, value);
                }
                else
                {
                    closeTokens++;
                }
                
                finish = closeTokens == 2 || value == '\0';
                if (finish) 
                {
                    kpush(subCommands, '\0');
                }
                
                value = *(arg++);
                // j++;
            }
        }
        index++;
    }
}

#endif
