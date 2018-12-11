/*	lcd.c
	Implementation for interfacing with Optrex LCD display.
*/

#include "lcd.h"

void LCD_Init()
{
	//Specification says to wait at least 15ms after startup before doing anything
	_delay_ms(20);
	
	//Function Set - Set up for 8-bit data, 2 line display, 5x7 dots
	LCD_cmd(LCD_Function_Set | 
			LCD_Function_Set_Data_Length_8 | 
			LCD_Function_Set_Lines_2 | 
			LCD_Function_Set_Font_7);

	//Clear Display
	LCD_cmd(LCD_Clear);
	
	//Turn ON Display, ON Cursor, and Blink Cursor
	LCD_cmd(LCD_Display_Control |
			LCD_Display_ON |
			LCD_Cursor_ON |
			LCD_Cursor_Blink);
}

void LCD_cmd(LCD_commands_t command)
{
	int Delay;
	int bLongDelay = FALSE;
	
	if((command == LCD_Start_Line_0) ||
	   (command == LCD_Start_Line_1))
	{
		Delay = LCD_DELAY_SHORT_us;
		bLongDelay = FALSE;
	}
	else
	{	
		Delay = LCD_DELAY_LONG_ms;
		bLongDelay = TRUE;
	}
	
	PORTC = command;
	PORTA |= PORTA_WRITE_INSTRUCTION_START_MASK;
	
	if(bLongDelay)
		_delay_ms(Delay);
	else
		_delay_us(Delay);
		
	PORTA &= PORTA_WRITE_INSTRUCTION_END_MASK;
}

void LCD_write_line(char* sLine, int LineNum)
{
	char* pChar = sLine;
	
	//We only have 2 lines, so do some error checking
	if(LineNum == 0)
		LCD_cmd(LCD_Start_Line_0);
	else if(LineNum == 1)
		LCD_cmd(LCD_Start_Line_1);
	else
		return;	//Danger Will Robinson!!
		
	while(*pChar != 0)
	{
		LCD_write_char(pChar++);
	}
}

void LCD_write_char(char* cChar)
{
	PORTC = *cChar;
	PORTA |= PORTA_WRITE_DATA_START_MASK;
	_delay_us(LCD_DELAY_SHORT_us);
	PORTA &= PORTA_WRITE_DATA_END_MASK;
}
