#if __TEST_PROMPT__
#include <stdlib.h>
#include <string.h>
#include "prompt_test.h"

const char* stdCommand = "dev -d 45";
const char* emptyCommand = "dev -v";
const char* fewCommands = "prompt -v -g 785 -h hello";

char _rx_buffer[32];

int Prompt_CanManageCommands() 
{
    int assert;
    char* command;
    memset(_rx_buffer, 0, 32);
    strncpy(_rx_buffer, stdCommand, 32);
    commandLine((char*)_rx_buffer, 32);
    command = getCommandKey();
    closeCommand();
    assert = strncmp(command, "dev", 8) == 0;
    return assert;
}

int Prompt_CanGetValueFromSubCommand()
{
    char* value;
    int assert;
    memset(_rx_buffer, 0, 32);
    strncpy(_rx_buffer, stdCommand, 32);
    commandLine((char*)_rx_buffer, 32);
    value = getSubCommandValue('d');
    closeCommand();
    assert = strncmp(value, "45", 64) == 0;
    return assert;
}

int Prompt_CanGetEmptyValueFromSubCommand()
{
    
    char* value;
    int assert;
    memset(_rx_buffer, 0, 32);
    strncpy(_rx_buffer, emptyCommand, 32);
    commandLine((char*)_rx_buffer, 32);
    value = getSubCommandValue('v');
    closeCommand();
    assert = strncmp(value, "", 64) == 0;
    return assert;
}

int Prompt_CanGetValuesFromSubCommands()
{
    char* value1;
    char* value2;
    char* value3;
    char* command;
    int assert1;
    int assert2;
    int assert3;
    int assert4;

    memset(_rx_buffer, 0, 32);
    strncpy(_rx_buffer, fewCommands, 32);
    commandLine((char*)_rx_buffer, 32);
    value1 = getSubCommandValue('v');
    assert1 = strncmp(value1, "", 64) == 0;
    value2 = getSubCommandValue('g');
    assert2 = strncmp(value2, "785", 64) == 0;
    value3 = getSubCommandValue('h');
    assert3 = strncmp(value3, "hello", 64) == 0;
    command = getCommandKey();
    assert4 = strncmp(command, "prompt", 8) == 0;
    closeCommand();
    
    return assert1 && assert2 && assert3 && assert4;
}

int executeTests() 
{
    int fails = 0;

    if (!Prompt_CanManageCommands()) 
    {
        fails++;
    }
    
    if (!Prompt_CanGetValueFromSubCommand()) 
    {
        fails++;
    }

    if (!Prompt_CanGetEmptyValueFromSubCommand()) 
    {
        fails++;
    }

    if (!Prompt_CanGetValuesFromSubCommands()) 
    {
        fails++;
    }
    
    return fails;
}

#endif
