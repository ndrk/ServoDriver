/*	lcd.h
	Defines for controlling Optrex LCD Display
*/

#include "common.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define LCD_LINE_LENGTH		20		//20x2 Display
#define LCD_LINES			2

#define LCD_DELAY_LONG_ms	16		//16000µs (16ms) delay
#define LCD_DELAY_SHORT_us	100		//100µs delay

//Bitmasks for LCD_Entry_Increment
#define LCD_Entry_Increment	2
#define LCD_Entry_Decrement	0
#define LCD_Shift			1

//Bitmasks for LCD_Display_Control
#define LCD_Display_ON		4
#define LCD_Display_OFF		0
#define LCD_Cursor_ON		2
#define LCD_Cursor_OFF		0
#define LCD_Cursor_Blink	1

//Bitmasks for LCD_Cursor_Shift
#define LCD_Cursor_Shift_Right	4
#define LCD_Cursor_Shift_Left	0

//Bitmasks for LCD_Display_Shift
#define LCD_Display_Shift_Right	12
#define LCD_Display_Shift_Left	8

//Bitmasks for LCD_Function_Set
#define LCD_Function_Set_Data_Length_8	16
#define LCD_Function_Set_Data_Length_4	0
#define LCD_Function_Set_Lines_2		8 
#define LCD_Function_Set_Lines_1		0
#define LCD_Function_Set_Font_10		4
#define LCD_Function_Set_Font_7			0

#define PORTA_WRITE_INSTRUCTION_START_MASK	0x40
#define PORTA_WRITE_INSTRUCTION_END_MASK	0xBF
#define PORTA_WRITE_DATA_START_MASK			0x41
#define PORTA_WRITE_DATA_END_MASK			0xBE


enum LCD_commands
{
	LCD_Clear			= 1,
	LCD_Home			= 2,
	LCD_Entry_Mode		= 4,
	LCD_Display_Control	= 8,
	LCD_Cursor_Shift	= 16,
	LCD_Display_Shift	= 16,
	LCD_Function_Set	= 32,
	LCD_Start_Line_0	= 128,
	LCD_Start_Line_1	= 192
};
	
typedef enum LCD_commands LCD_commands_t;

void LCD_Init();
void LCD_cmd();
void LCD_write_line(char* sLine, int Line);
void LCD_write_char(char* cChar);
