#include <stdbool.h>
#include <pic18.h>
#include "converters.h"

void getBCD(char* digits, int bytes, int inputNumber) 
{
    int step = 0,
            bitsQty = 0,
            window = 1,
            shifterCounter = inputNumber;
    
    for (int i = 0; i < bytes; i++) 
    {
        digits[i] = '\0';
    }
    
    while (shifterCounter) 
    {
        shifterCounter >>= 1;
        window <<= 1;
        bitsQty++;
    }
    
    while (step < bitsQty) 
    {
        int temp;
        int lastStep = bitsQty - 1;
        inputNumber <<= 1;
        temp = inputNumber & window;
        temp >>= bitsQty;
        
        for (int index = 0; index < bytes; index++) 
        {
            if (index > 0) 
            {
                temp = (digits[index - 1] & 16) >> 4;
                digits[index - 1] = digits[index - 1] & 15;
            }
            
            digits[index] = (char)(digits[index] << 1);
            digits[index] = (char)(digits[index] | temp);
            
            char valid  = LOW_BYTE(digits[index]) & 15;
            
            if (valid >= 5 && (lastStep != step)) 
            {
                digits[index] = digits[index] + 3;
            }
        }
        
        step++;
    }
}
