/* 
 * File:   lcd.h
 * Author: Nelson
 *
 * Created on March 20, 2022, 3:50 PM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif
#define CLEAR_DISPLAY 1
#define RETURN_HOME 2
/*----ENTRY_MODE_SET-----*/
#define ENTRY_MODE_SET 4
#define ENTRY_CURSOR_INCREASE 2
#define ENTRY_SHIFT_ENABLE 1
/*----DISPLAY_ON_OFF-----*/
#define DISPLAY_ON_OFF 8
#define TURN_ON_DISPLAY 4
#define SHOW_CURSOR 2
#define CURSOR_BLINKING 1
/*----CURSOR_DISPLAY-----*/
#define CURSOR_DISPLAY 16
#define CURSOR_MOVE_RIGHT 1
#define DISPLAY_MOVE_LEFT 2
#define DISPLAY_MOVE_RIGHT 3 
/*----FUNCTION_SET-----*/
#define FUNCTION_SET 32
#define MODE_8BITS 16
#define TWO_LINES 8
#define FONT_5x11 4
/*----SET CGRAM_ADDRESS-----*/
#define SET_CG_ADDRESS 64
/*----SET DDRAM_ADDRESS-----*/
#define SET_DD_ADDRESS 128
/*----READ_BUSY_FLAG-----*/
#define READ_BUSY_FLAG 256
/*----WRITE_DATA_RAM-----*/
#define DATA_RAM 512
#define READ_DATA 256

extern int write_command(int command);
extern int lcd_wait();
extern void InitLcd();
extern int lcd_putchar(char c);
extern int lcd_write(char c);
extern void lcd_clear();
extern void lcd_rewind();
#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

