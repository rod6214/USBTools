#include <xc.h>
#include "adc.h"

void initAdc() 
{
    TRISAbits.RA0 = 1;
    ADCON0bits.CHS = 0;
    ADCON1bits.VCFG = 0;
    ADCON1bits.PCFG = 14;
    ADCON2bits.ADFM = 1;
    ADCON2bits.ACQT = 7;
    ADCON2bits.ADCS = 6;
    ADCON0bits.ADON = 1;
    ADCON0bits.GODONE = 1;
}

int getADCValue() 
{
    int result = ADRESH;
    result = (result << 8) | ADRESL;
    return result;
}

