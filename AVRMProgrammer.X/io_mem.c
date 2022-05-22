/*
 * io_mem.c
 *
 * Created: 5/1/2022 2:29:07 PM
 *  Author: Nelson
 */ 
# define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>

void set_address_low_Z() 
{
	DDRC = 0;
	_delay_loop_1(2);
	PORTC = 0;
	_delay_loop_1(2);
}

void set_address_high_Z()
{
	DDRA = 0;
	_delay_loop_1(2);
	PORTA = 0;
	_delay_loop_1(2);
}

void set_address_low(char addressl) 
{
	DDRC = 255;
	_delay_loop_1(2);
	PORTC = addressl;
	_delay_loop_1(2);
}

void set_address_high(char addressh)
{
	DDRA = 255;
	_delay_loop_1(2);
	PORTA = addressh;
	_delay_loop_1(2);
}
	
void set_data_high_z() 
{
	DDRD = DDRD & (~252);
	_delay_loop_1(1);
	DDRB = DDRB & (~24);
	_delay_loop_1(1);
	PORTD = PORTD & (~252);
	_delay_loop_1(1);
	PORTB = PORTB & (~24);
	_delay_loop_1(1);
}

char get_data() 
{
	char datah = 0;
	char datal = 0;
	int i = 0;
	while (i < 100) 
	{
		datah = PIND & 252;
		datal = (PINB & 24) >> 3;
		i++;
		_delay_us(1);
	}
	return datah | datal;
}

void set_data(char data) 
{
	DDRD = DDRD | 252;
	_delay_loop_1(20);
	DDRB = DDRB | 24;
	_delay_loop_1(20);
	PORTD = PORTD | (data & 252);
	PORTB = PORTB | ((data & 3) << 3);
	_delay_loop_1(20);
}
	
void set_chip_enable(int value)
{
	if (!value) 
	{
		PORTB = PORTB | ((1 << PB0));
	}
	else 
	{
		PORTB = PORTB & (~(1 << PB0));
	}
	_delay_loop_1(20);
}

void set_output_enable(int value) 
{
	if (!value)
	{
		PORTB = PORTB | ((1 << PB1));
	}
	else
	{
		PORTB = PORTB & (~(1 << PB1));
	}
	_delay_loop_1(20);
}

void set_write_enable(int value)
{
	if (!value)
	{
		PORTB = PORTB | ((1 << PB2));
	}
	else
	{
		PORTB = PORTB & (~(1 << PB2));
	}
	_delay_loop_1(20);
}

void init_ctrl_mem()
{
	PORTB = PORTB | ((1 << PB0) | (1 << PB1) | (1 << PB2));
	DDRB = DDRB | ((1 << DDB0) | (1 << DDB1) | (1 << DDB2));
	_delay_loop_1(1);
	set_address_low_Z();
	set_address_high_Z();
	set_data_high_z();
}

