#include <xc.h>
#include "printer.h"
// Commands
#define PWM_MODULE 0x22
#define MEMORY 0x52
#define VERSION 0x44

// Functions
// PWM_MODULE
#define SET_PWM_CCP_REGISTER 0x33
#define GET_PWM_CCP_REGISTER 0x32
// MEMORY
#define SET_MEMORY 0x12
#define GET_MEMORY 0x13

typedef struct ToolStream 
{
    HANDLE ptr;
} ToolStream_t;

void InitDevTool(STREAM stream) {}
void ExcuteCommand(STREAM stream) {}
void ReadCommand(STREAM stream) {}
