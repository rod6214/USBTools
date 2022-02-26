#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "prompt.h"
#include "kernel.h"

#if __DEVTOOL__
const char* message_list[] = 
{
    "DevTool for PIC18, your free communication program\n",
    "version 0.0.0.1\n",
    "You can start by typing a command:\n",
    "Listening...\n",
    "Are you sure to quick?",
};

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
    command = CreateList(8);
    subCommands = CreateList(64);
    processCommands(arg, command, subCommands);
//    
//    char* result = getSubCommandValue('d');
//    
//    PORTB = result[0];
}

void* getCommand() 
{
    return command;
}

void* getSubcommands() 
{
    return subCommands;
}

char* getSubCommandValue(char command) 
{
    {
        int j = 0;
        int closeTokens = 0;
        foreach(subCommands)
        {
            char c = kgetchar(subCommands);
            
            if (c == '-' && closeTokens == 0 || closeTokens == 1 && c == command) 
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
            }
            
            j++;
        }
    }
    return (char)-1;
}

void processCommands(char* arg, void* command, void* subCommands) 
{
    char temp[16];
    int index = 0;
    int state;
    
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
            int j = 0;
            int finish = FALSE;
            int closeTokens = 0;
            
            while(!finish) 
            {
                if (value != ' ') 
                {
                    kpush(subCommands, value);
                }
                
                value = *(arg++);
                
                if (value == ' ') 
                {
                    closeTokens++;
                }
                
                finish = closeTokens == 2 || value == '\0';
                if (finish) 
                {
                    kpush(subCommands, '\0');
                }
                
                j++;
            }
        }
        index++;
    }
}

#endif
