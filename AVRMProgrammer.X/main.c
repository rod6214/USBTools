/*
 * AT90Programmer.c
 *
 * Created: 5/1/2022 12:31:53 PM
 * Author : Nelson
 */ 
# define F_CPU 4000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "io_mem.h"

#define MAX_MEMORY 128
char write_buffer[MAX_MEMORY];
char read_buffer[MAX_MEMORY];
int read_idx = 0;
int write_idx_z = 0;
int write_idx = 0;
int bytes = 0;
int offset = 0;
int step = NULL;
char command = 0;
int data_sent = FALSE;
int isBusy = FALSE;

void write_mem(int address, char data);
char read_mem(int address);
char usart_receive();
void usart_send(char data);
void usart_start();
void usart_stop();

#define wait_host() while(!data_sent); data_sent=FALSE

ISR(USART_TX_vect)
{
	cli();
	data_sent = TRUE;
	sei();
}

ISR(USART_RX_vect)
{
	/************************************************************************************************/
	/* Max length per FRAME is 3k*8                                                                 */
	/*  ___0___  _____1____  _____2____  _________3_________              ________N_________        */
	/* /COMMAND\/  BYTES L \/  BYTES H \/		 3k         \............/		 3k         \       */
	/* \ 8bit  /\   8bit   /\   8bit   /\      FRAME        /            \       FRAME      /       */
	/************************************************************************************************/
	
	cli();
	char received_data = usart_receive();

//	if (isBusy)
//		return;

	if (((received_data == READ_MEMORY) || 
	(received_data == WRITE_MEMORY) ||
	(received_data == STORE_DATA)) && (step == NULL)) 
	{
		step = COMMAND_STEP;
	}

	switch(step)
	{
		case COMMAND_STEP:
		{
			command = (int)(0xff & received_data);
			step = LOW_BYTE_STEP;
		}
		break;
		case LOW_BYTE_STEP:
		{
			bytes = (int)(0xff & received_data);
			step = HIGH_BYTE_STEP;
		}
		break;
		case HIGH_BYTE_STEP:
		{
			bytes |= ((int)(0xff & received_data) << 8);
			step = LOW_OFFSET_STEP;
		}
		break;
		case LOW_OFFSET_STEP:
		{
			offset = (int)(0xff & received_data);
			step = HIGH_OFFSET_STEP;
		}
		break;
		case HIGH_OFFSET_STEP:
		{
			offset |= ((int)(0xff & received_data) << 8);
			step = DATA_STEP;
			write_idx_z = 0;
			write_idx = 0;
		}
		break;
		case DATA_STEP:
		{
			if (command == WRITE_MEMORY)
			{
				command = WRITE_MEMORY_EXEC;
			}
			else if (command == STORE_DATA) 
			{
				write_buffer[write_idx] = received_data;
				write_idx++;
				if (write_idx > bytes) 
				{
					command = FINISH_STORE_DATA;
				}
			}
		}
		break;
		default:
		{
			step = NULL;
		}
		break;
	}
	sei();
}

char read_mem(int address) 
{
	set_data_high_z();
	char addressl = (char)(0xff & address);
	char addressh = (char)(((0xff00) & address) >> 8);
	set_address_low(addressl);
	set_address_high(addressh);
	_delay_loop_1(20);
	set_chip_enable(TRUE);
	_delay_loop_1(20);
	set_output_enable(TRUE);
	_delay_loop_1(20);
	char data = get_data();
	_delay_loop_1(100);
	set_chip_enable(FALSE);
	_delay_loop_1(20);
	set_output_enable(FALSE);
	//_delay_loop_1(2);
	//set_address_low_Z();
	//set_address_high_Z();
	return data;
}

void write_mem(int address, char data) 
{
	char addressl = LOW_BYTE(address);
	char addressh = HIGH_BYTE(address);
	set_address_low(addressl);
	set_address_high(addressh);
	_delay_loop_1(20);
	set_output_enable(FALSE);
	_delay_loop_1(20);
	set_chip_enable(TRUE);
	_delay_loop_1(20);
	set_write_enable(TRUE);
	_delay_loop_1(100);
	set_data(data);
	_delay_loop_1(100);
	set_write_enable(FALSE);
}

void deactivate_mem_proc() 
{
	_delay_loop_1(100);
	set_chip_enable(FALSE);
	_delay_loop_1(20);
	set_address_low_Z();
	set_address_high_Z();
	_delay_loop_1(2);
	set_data_high_z();
	_delay_loop_1(2);
}

void usart_start() 
{
	/************************************************************************/
	/*   UBRR0 = fosc/(16*BAUD) - 1                                         */
	/************************************************************************/
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE) | (1 << TXCIE) | (1 << UCSZ2);
	_delay_loop_1(1);
	UBRRL = 25;
	_delay_loop_1(1);
}

void usart_stop()
{}

void usart_send(char data)
{
	UCSRB = UCSRB | (1 << TXB8);
	UDR = data;
}

char usart_receive()
{
	return UDR;
}

void config()
{
	usart_start();
	init_ctrl_mem();
	_delay_loop_1(100);
	step = NULL;
	//isBusy = 0;
	write_idx_z = 0;
	write_idx = 0;
	read_idx = 0;
	sei();
}

void loop() 
{
	if (step == DATA_STEP)
	{
		if (command == READ_MEMORY)
		{
			/************************************************************************/
			/* NOTE: Only we can iterate until 28671, to extend this consider use   */
			/* another data type other than int in the for.                         */
			/************************************************************************/		
			for (int i = 0; i < 64; i++)
			{
				// Send data to the host
				char data = read_mem(i + offset);
				_delay_ms(1);
				usart_send(data);
				wait_host();
			}
			
			// Finish the process
			read_idx=0;
			step=NULL;
			command=NULL;	
			deactivate_mem_proc();
			//for (int i = 0; read_idx < bytes && i < MAX_MEMORY; i++, read_idx++)
			//{
				//// Send data to the host
				//char data = read_mem(read_idx + offset);
				//_delay_loop_1(1);
				//usart_send(data);
				//wait_host();
			//}
			//
			//if (read_idx >= bytes)
			//{
				//// Finish the process
				//read_idx=0;
				//step=NULL;
				//command=NULL;
			//}
		}
		else if (command == WRITE_MEMORY_EXEC)
		{
			//char data = write_buffer[0];
			//write_mem(write_idx, data);
			  for (int i = 0; i < 64; i++)
			  {
			  	// Write memory
			  	char data = write_buffer[i];
			  	write_mem(i + offset, data);
				_delay_us(5);
			  }
			  
			  _delay_ms(10);
			//write_idx++;
			 //if (write_idx >= bytes)
			 //{
			 	//// Finish the process
			 	//write_idx=0;
			 	//step=NULL;
				//command=NULL;
			 //}
			 //else 
			 //{
				//command = WRITE_MEMORY; 
			 //}
			
			// Send a ready command to the host or ACK
			// Finish the process
			usart_send(ACK);
			wait_host();
			isBusy = FALSE;
			write_idx=0;
			step=NULL;
			command=NULL;
			deactivate_mem_proc();
		}
		else if (command == FINISH_STORE_DATA) 
		{
			usart_send(ACK);
			wait_host();
			step=NULL;
			command=NULL;
			isBusy = FALSE;
            write_idx = 0;
		}
		// else if (command == GET_MORE)
		// {
		// 	command = WRITE_MEMORY;
		// 	usart_send(GET_MORE);
		// 	wait_host();
		// }
	}
}

int main(void)
{
	config();
    /* Replace with your application code */
    while (1) 
    {
		loop();
    }
}

