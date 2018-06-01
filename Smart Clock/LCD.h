/*
 * LCD.h
 *
 * Created: 6/1/2018 3:59:56 PM
 *  Author: yosef
 */ 


#ifndef LCD_H_
#define LCD_H_

void LCD_Init(void);
void LCD_SendData(unsigned char data);
void LCD_Send_Command(unsigned char cmd);
void LCD_Clear(void);
void LCD_GotoXY(unsigned char x, unsigned char y);
void LCD_SendString(char *str);
void LCD_SendString_XY(char *str, unsigned char x, unsigned char y);
char* convertData(unsigned int var);



#endif /* LCD_H_ */