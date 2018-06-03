/*
 * LCD.h
 *
 * Created: 6/3/2018 7:31:06 AM
 *  Author: moham
 */ 


#ifndef LCD_H_
#define LCD_H_

void pinChange(int a, int b);
void Lcd4_Port(char a);
void Lcd4_Cmd(char a);
void Lcd4_Clear();
void Lcd4_Set_Cursor(char a, char b);
void Lcd4_Init();
void Lcd4_Write_Char(char a);
void Lcd4_Write_String(char *a);

#endif /* LCD_H_ */