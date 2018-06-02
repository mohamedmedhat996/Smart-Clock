/*
 * LCD.c
 *
 * Created: 6/1/2018 4:00:09 PM
 *  Author: yosef
 */ 
#define F_CPU 1000000l 
#include "LCD.h"
#include <avr/io.h>
#include <util/delay.h>
//#include "macros.h"

void LCD_Init(void)
{
	// Set Data Port to be O/P
	DDRD = 0xFF;
	PORTD = 0x00;

	// Set Control Port to be O/P
	DDRB = 0x07;
	PORTB = 0x01;

	// Init LCD
	LCD_Send_Command(0x38); // 8 bit mode
	// LCD_on
	LCD_Send_Command(0x0C); // turn on LCD
}

void LCD_SendData(unsigned char data)
{
	// Put Data on Port
	PORTD = data;
	// Set R/W to W
	PORTB = PORTB & 0xFD; // ~(1<<RS_LOC)
	// Set RS to Data
	PORTB = PORTB | 0x04;

	// E to be 0
	PORTB = PORTB & 0xFE;
	// Delay
	_delay_ms(10);
	// E to be 1
	PORTB = PORTB | 0x01;
	
	// Delay to let LCD Display Data
	_delay_ms(100);
}

void LCD_Send_Command(unsigned char cmd)
{
	// Put Cmd on Port
	PORTD = cmd;
	// Set R/W to W
	PORTB = PORTB & 0xFD;
	// Set RS to Command
	PORTB = PORTB & 0xFB;

	// E to be 0
	PORTB = PORTB & 0xFE;
	// Delay
	_delay_ms(1);
	// E to be 15022
	PORTB = PORTB | 0x01;
	
	// Delay to let LCD Execute Command
	_delay_ms(100);
}

void LCD_Clear(void)
{
	// Send Clr command to LCD
	LCD_Send_Command(0x01);
}

void LCD_GotoXY(unsigned char x, unsigned char y)
{
	if(y == 0)
	{
		LCD_Send_Command(0x80 + x);
	}
	else
	{
		LCD_Send_Command(0x80 + 0x40 + x);
	}
}

void LCD_SendString(char *str)
{
	while(*str != '\0')
	{
		LCD_SendData(*str);
		str++;
	}
}

// select the row and column of LCD
void LCD_SendString_XY(char *str, unsigned char x, unsigned char y)
{
	LCD_GotoXY(x, y);
	LCD_SendString(str);
}

// convert integers to string then mydata is pointer to string
char* convertData(unsigned int var)
{
	static char data[17];
	char* arr = &data[16];

	*arr = '\0';

	while(var != 0)
	{
		arr--;
		*arr = (var%10) + '0';
		var = var / 10;
	}
	return arr;
}