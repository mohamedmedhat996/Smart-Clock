/*
 * Smart Clock.c
 *
 * Created: 6/1/2018 7:42:36 PM
 * Author : Mohamed Medhat
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LCD.h"

#define SET_BIT(ADDRESS, BIT) ADDRESS |= (1<<BIT)
#define RESET_BIT(ADDRESS, BIT) ADDRESS &= ~(1<<BIT)
#define TOGGLE_BIT(ADDRESS, BIT) ADDRESS ^= (1<<BIT)
#define READ_BIT(ADDRESS, BIT) ((ADDRESS & (1<<BIT))>>BIT)

/*Global Variables Declarations*/
unsigned char HOURS = 0;
unsigned char MINUTES = 0;
unsigned char SECONDS = 0;

unsigned char NewHours;
unsigned char NewMinutes;
unsigned char NewSeconds;


void CTC_mode ()
{
	// Set the Timer Mode to CTC
	TCCR0A |= (1 << WGM01);

	// Set the value that you want to count to
	OCR0A = 0x3c;
	
	// start the timer (No Prescalling)
	TCCR0B |= (1 << CS00);
	
	TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vector

	sei();         //enable interrupts
}

void Increase (int data)
{
	if(data<59) data++;
	else data=0;
}

void Decrease (int data)
{
	if(data>0) data--;
	else data=60;
}

void Done (int )
{
	
}

int main(void)
{
	char mode;
	char toggle;
	char up;
	char down;
	char ok;
	CTC_mode();
    while (1) 
    {
		mode=PINC; //Default in Clock mode
		READ_BIT(mode,0);
		toggle=PINC;
		READ_BIT(toggle,1);
		up=PINC;
		READ_BIT(up,2);
		down=PINC;
		READ_BIT(down,3);
		ok=PINC;
		READ_BIT(ok,4);
		unsigned char data;
		
		if(mode==0)
		{
			//change from mode to anther
		}
		if(toggle==0)
		{
			//Select (hours or minutes or seconds)
			// data will be an (hours or minutes or seconds)
		}
		if(up==0) Increase(data);
		if(down==0) Decrease(data);
		if(ok == 0) Done();
		
		//display time
    }
}

ISR (TIMER0_COMPA_vect)  // timer0 overflow interrupt
{
	SECONDS++;
	
	if(SECONDS==60)
	{
		SECONDS=0;
		MINUTES++;	
	}
	
	if(MINUTES==60)
	{
		MINUTES=0;
		HOURS++;
	}
	
	if(HOURS>23) HOURS=0;
	
	//display time
}