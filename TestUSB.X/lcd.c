/*
 * File:   lcd.c
 * Author: Nelson
 *
 * Created on March 20, 2022, 3:50 PM
 */
#define _XTAL_FREQ 48000000

#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include "kernel.h"
#include "lcd.h"
#include "cstream.h"

#if __LCD__
volatile Stream_t stream;
static int position = 0;
static int sendPulse();

void initLcd()
{
    PORTAbits.RA1 = 0;
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 0;
    PORTCbits.RC2 = 0;
    PORTCbits.RC6 = 1;
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    TRISCbits.RC2 = 0;
    TRISCbits.RC6 = 0;
    TRISAbits.RA1 = 0;
    
    __delay_us(10);
    PORTAbits.RA1 = 1;
    __delay_ms(50);
    
    write_command(DISPLAY_ON_OFF | TURN_ON_DISPLAY);
    lcd_wait();
    write_command(FUNCTION_SET | MODE_8BITS);
    lcd_wait();
}

void lcd_rewind() 
{
    position = 0;
}
//
//void* lcd_getStream() 
//{
//    return (void*)&stream;
//}
//
int lcd_putchar(char c) 
{
    int address = 0;
    
    if (position == 0) 
    {
        write_command(SET_DD_ADDRESS | 0x00);
        lcd_wait();
    }
    else if (position == 16) 
    {
        write_command(SET_DD_ADDRESS | 0x10);
        lcd_wait();
    }
    else if (position == 32) 
    { 
        write_command(SET_DD_ADDRESS | 0x08);
        lcd_wait();
    }
    else if (position == 48) 
    { 
        write_command(SET_DD_ADDRESS | 0x18);
        lcd_wait();
    }
    
    if ((position >= 0) && (position < 64))
    {
        address = lcd_write(c);
        position++;
    }
    
    return address;
}

int lcd_write(char c) 
{
    int address = 0;
    write_command(DATA_RAM | c);
    address = lcd_wait();
    return address;
}
//
//char lcd_getchar() 
//{
//}
//
void lcd_clear() 
{
//    write_command(CLEAR_DISPLAY);
//    lcd_wait();
//    __delay_ms(5);
//    write_command(RETURN_HOME);
//    lcd_wait();
    lcd_rewind();
}


int write_command(int command) 
{
    /**
     * RC0 RS
     * RC1 R/W
     * RC2 E
     */
    int result = 0;
    PORTCbits.RC0 = (command & 512) > 0;
    PORTCbits.RC1 = (command & 256) > 0;
    
    if (!PORTCbits.RC1) 
    {
        TRISB = 0;
        PORTB = LOW_BYTE(command);
    }
    
    result = sendPulse();
    return result;
}

int lcd_wait() 
{
    int address = 0;
    TRISB = 255;
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 1;
    address = sendPulse();
    while(PORTBbits.RB7);
    PORTCbits.RC1 = 0;
    return address;
}

static int sendPulse() 
{
    int result = 0;
    PORTCbits.RC2 = true;
    __delay_us(10);
    result = PORTB & 127;
    PORTCbits.RC2 = false;
    __delay_us(10);
    return result;
}


#endif
