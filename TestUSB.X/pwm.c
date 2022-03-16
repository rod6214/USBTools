#if __PWM_DRIVER__
#include <pic18.h>
#include <xc.h>
#include <stdbool.h>
#include "pwm.h"

void pwm_setFreq(PWM_t* p, int freq) 
{
    p->Freq = freq;
}

void pwm_setCCPPort(PWM_t* p, int port) 
{
    p->CPP_Port = port;
}

void pwm_turnOnCPPModules(PWM_t* p, int modules) 
{
    p->CPP_Modules = modules;
}

int pwm_getDutyCicle(PWM_t* p) 
{
    return p->DutyCicle;
} 

void pwm_init(PWM_t* conf)
{
    T2CON = 0;
    asm("BANKSEL (T2CON) ");
    asm("BSF " TMR2ON_bit);

    if (conf->Freq == PWM_FREQ_50K) 
    {
        PR2 = LOW_BYTE(255);
        asm("BANKSEL (T2CON) ");
        asm("BCF " T2CKPS0_bit);
        asm("BCF " T2CKPS1_bit);
    }

    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP2) 
    {
       CCP2CON = 0;
       asm("BANKSEL (CCP2CON) ");
       asm("BSF " CCP2M0_bit);
       asm("BSF " CCP2M1_bit);
       asm("BSF " CCP2M2_bit);
       asm("BSF " CCP2M3_bit);
       CCP2CONbits.CCP2M0 = 1;
       CCP2CONbits.CCP2M1 = 1;
       CCP2CONbits.CCP2M2 = 1;
       CCP2CONbits.CCP2M3 = 1;
    }
    
    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP1)
    {
       CCP1CON = 0;
       CCP1CONbits.CCP1M0 = 1;
       CCP1CONbits.CCP1M1 = 1;
       CCP1CONbits.CCP1M2 = 1;
       CCP1CONbits.CCP1M3 = 1;
    }
    
    if (LOW_BYTE(conf->CPP_Port) & PWM_RC1) 
    {
        PORTCbits.CCP2 = 0;
        TRISCbits.TRISC1 = 0;
    }
    if (LOW_BYTE(conf->CPP_Port) & PWM_RC2) 
    {
        PORTCbits.CCP1 = 0;
        TRISCbits.TRISC2 = 0;
    }
    if (LOW_BYTE(conf->CPP_Port) & PWM_RB3) 
    {
        PORTBbits.CCP2_PA2 = 0;
        TRISBbits.TRISB3 = 0;
    }
    
    conf->IsActivated = true;
}

void pwm_setPulse(PWM_t* conf, int percentage)
{
    unsigned char lsb = LOW_BYTE(percentage);
    unsigned char msb = HIGH_BYTE(percentage) & 3;
    conf->DutyCicle = percentage;
    
    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP2) 
    {
        asm("BANKSEL (CCP2CON) ");
        if ((msb & 1)) 
        {
            asm("BSF " DC2B0_bit);
        }
        
        if ((msb & 2)) 
        {
            asm("BSF " DC2B1_bit);
        }
        CCPR2L = lsb;
    }
    
    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP1)
    {
        asm("BANKSEL (CCP1CON) ");
        if ((msb & 1)) 
        {
            asm("BSF " DC1B0_bit);
        }
        
        if ((msb & 2)) 
        {
            asm("BSF " DC1B1_bit);
        }
        CCPR1L = lsb;
    }
}

void pwm_finish(PWM_t* conf) 
{
    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP2) 
    {
        asm("BANKSEL (CCP1CON) ");
        asm("BCF " CCP2M0_bit);
        asm("BCF " CCP2M1_bit);
        asm("BCF " CCP2M2_bit);
        asm("BCF " CCP2M3_bit);
    }
    
    if (LOW_BYTE(conf->CPP_Modules) & PWM_CPP1)
    {
        asm("BANKSEL (CCP2CON) ");
        asm("BCF " CCP1M0_bit);
        asm("BCF " CCP1M1_bit);
        asm("BCF " CCP1M2_bit);
        asm("BCF " CCP1M3_bit);
    }
    
    if (LOW_BYTE(conf->CPP_Port) & PWM_RC1) 
    {
        TRISCbits.TRISC1 = 1;
    }
    if (LOW_BYTE(conf->CPP_Port) & PWM_RC2) 
    {
        TRISCbits.TRISC2 = 1;
    }
    if (LOW_BYTE(conf->CPP_Port) & PWM_RB3) 
    {
        TRISBbits.TRISB3 = 1;
    }
    
    conf->IsActivated = false;
}
#endif
