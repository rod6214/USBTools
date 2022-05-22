/*
 * io_mem.h
 *
 * Created: 5/1/2022 3:14:31 PM
 *  Author: Nelson
 */ 


#ifndef IO_MEM_H_
#define IO_MEM_H_

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#define LOW_BYTE(x) (0x00FF & x)
#define HIGH_BYTE(x) ((0xFF00 & x) >> 8)
#define SET_WORD_LOW(x, y) (x = 0x00FF & y)
#define SET_WORD_HIGH(x, y) (x = 0x0FF00 | (y << 8))
#define READ_MEMORY 0x0A
#define WRITE_MEMORY 0x0B
#define ACK 0x0C
#define GET_MORE 0x0D
#define STORE_DATA 0x0E
#define FINISH_STORE_DATA 0x0F
#define NULL 0
#define COMMAND_STEP 1
#define LOW_BYTE_STEP 2
#define HIGH_BYTE_STEP 3
#define HIGH_OFFSET_STEP 4
#define LOW_OFFSET_STEP 5
#define DATA_STEP 6
#define WRITE_MEMORY_EXEC 7

extern void set_address_low(char addressl);
extern void set_address_high(char addressh);
extern void set_address_high_Z();
extern void set_address_low_Z();
extern void set_data_high_z();
extern char get_data();
extern void set_data(char data);
extern void init_ctrl_mem();
extern void set_chip_enable(int value);
extern void set_output_enable(int value);
extern void set_write_enable(int value);

#endif /* IO_MEM_H_ */