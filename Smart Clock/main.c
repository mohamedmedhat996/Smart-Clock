/*
 * Smart Clock.c
 *
 * Created: 6/1/2018 7:42:36 PM
 */
#define buzzer_bit 0
#define buzzer_port PORTB
#define timer_vect TIMER1_COMPA_vect
#define mode_bit 2
#define mode_port PORTD
#define mode_vect INT0_vect
#define temp_bit
#define temp_port
#define toggle_bit 3
#define toggle_port PORTD
#define toggle_vect INT1_vect
#define ok_bit 4
#define ok_port PINC
#define ok_vect
#define up_bit 1
#define up_port PORTD
#define up_vect PCINT2_vect
#define down_bit 3
#define down_port PORTC
#define down_vect PCINT1_vect
#define LCD_port
#define F_CPU 16000000ul

#define SET_BIT(ADDRESS, BIT) ADDRESS |= (1<<BIT)
#define RESET_BIT(ADDRESS, BIT) ADDRESS &= ~(1<<BIT)
#define TOGGLE_BIT(ADDRESS, BIT) ADDRESS ^= (1<<BIT)
#define READ_BIT(ADDRESS, BIT) ((ADDRESS & (1<<BIT))>>BIT)
#define set_as_output(bit,DDRX) {DDRX |= _BV(bit);}
#define set_as_input(bit,DDRX) 	{DDRX &= ~_BV((bit));}

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LCD/LCD.h"

unsigned short am_pm_t = 0;
unsigned short am_pm_a = 0;
unsigned short mode = 0;
unsigned short n_modes = 2;

unsigned short n_toggles = 0;
unsigned short toggle_count = 0;

unsigned short temperature_c = 0;

unsigned long time = 0;
unsigned short hour = 1;
unsigned short minute = 0;
unsigned short second = 0;

unsigned short days = 1;
unsigned short day = 1;
unsigned short year = 1;
unsigned short month = 1;

unsigned long alarm_count = 0;
unsigned long alarm_value = 0;
unsigned short alarm_state = 0;
unsigned short fire_alarm = 0;
unsigned short alarm_hour = 1;
unsigned short alarm_minute = 0;
unsigned short alarm_second = 0;

void DisplayDate(int x,int y){
	Lcd4_Set_Cursor(x,y);
	char Year[4], Month[2], Day[2];
	itoa((int)year,Year,10);
	itoa((int)month,Month,10);
	itoa((int)day,Day,10);
	
	Lcd4_Write_Char(Year[0]);
	Lcd4_Write_Char(Year[1]);
	Lcd4_Write_Char(Year[2]);
	Lcd4_Write_Char(Year[3]);
	Lcd4_Write_Char('/');
	Lcd4_Write_Char(Month[0]);
	Lcd4_Write_Char(Month[1]);
	Lcd4_Write_Char('/');
	Lcd4_Write_Char(Day[0]);
	Lcd4_Write_Char(Day[1]);
}

void DisplayAlarm(int x,int y){
	Lcd4_Set_Cursor(x,y);
	char Hour[2], Minute[2] , Second[2];
	itoa((int)alarm_hour,Hour,10);
	itoa((int)alarm_minute,Minute,10);
	itoa((int)alarm_second,Second,10);
	
	Lcd4_Write_Char(Hour[0]);
	Lcd4_Write_Char(Hour[1]);
	Lcd4_Write_Char(':');
	Lcd4_Write_Char(Minute[0]);
	Lcd4_Write_Char(Minute[1]);
	Lcd4_Write_Char(':');
	Lcd4_Write_Char(Second[0]);
	Lcd4_Write_Char(Second[1]);
	Lcd4_Write_Char(' ');
	
	if (am_pm_a==0) 
		Lcd4_Write_String("AM");
	else 
		Lcd4_Write_String("PM");
	Lcd4_Write_Char(' ');
	
	if(alarm_state == 1)
		Lcd4_Write_String("ON");
	else
		Lcd4_Write_String("OFF");
}

void DisplayTime(int x,int y){
	Lcd4_Set_Cursor(x,y);
	char Hour[2], Minute[2] , Second[2];
	itoa((int)hour,Hour,10);
	itoa((int)minute,Minute,10);
	itoa((int)second,Second,10);
	
	Lcd4_Write_Char(Hour[0]);
	Lcd4_Write_Char(Hour[1]);
	Lcd4_Write_String(":");
	Lcd4_Write_Char(Minute[0]);
	Lcd4_Write_Char(Minute[1]);
	Lcd4_Write_String(":");
	Lcd4_Write_Char(Second[0]);
	Lcd4_Write_Char(Second[1]);
	Lcd4_Write_String(" ");
	
	if (am_pm_t==0)
		Lcd4_Write_String("AM");
	else
		Lcd4_Write_String("PM");
}

void DisplayTemperature(int x, int y){
	Lcd4_Set_Cursor(x,y);
	char Temperature[3];
	itoa((int)temperature_c,Temperature,10);
	Lcd4_Write_Char(Temperature[0]);
	Lcd4_Write_Char(Temperature[1]);
	Lcd4_Write_Char(Temperature[2]);
	Lcd4_Write_Char('*');
	Lcd4_Write_Char('C');
}

void DisplayMode0(){
	//display date, temperature_c, alarm_state, time
	Lcd4_Clear();
	DisplayDate(1, 0);
	DisplayTemperature(1,11);
	DisplayTime(2,0);
}

void DisplayMode1(){
	//display, temperature_c, alarm state, alarm
	Lcd4_Clear();
	DisplayDate(1,0);
	DisplayTemperature(1,11);
	DisplayAlarm(2,0);
}

unsigned short toggle(unsigned short d){
    if(d == 0)
        d=1;
    else
        d=0;
    return d;
}

void ADC_intialzation ()
{
	//Internal 1.1V Voltage Reference with external capacitor at AREF pin
	ADMUX |=(1<<REFS0)|(1<<REFS1);
	//ADC5
	ADMUX |= (1<<MUX0)|(1<<MUX2);
	//So for better accuracy of digital output we have to choose lesser frequency.
	//For lesser ADC clock we are setting the presale of ADC to maximum value (128).
	//Since we are using the internal clock of 1MHZ, the clock of ADC will be (1000000/128).
	ADCSRA |=(1<<ADEN)|(1<<ADATE)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}

void CTC_mode ()
{
	// Set the Timer Mode to CTC with 256 From scale
	TCCR1A = 0;
	TCCR1B |= (1<<WGM12)|(1<<CS12);
	
	// initialize counter
	TCNT1 = 0;
	
	// initialize compare value
	//OCR1A = 0x7A11;
	OCR1A = 0x408;
	
	//OCRn =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
	//OCRn =  [ (16 M / 256) * 1 ] - 1 = 62499 = F423 Hex
	// Set the value that you want to count to
	//OCR1AH = 0xF4;
	//OCR1AL =0x23;

	TIMSK1 |= (1 << OCIE0A);    //Set the ISR COMPA vector

	sei();         //enable interrupts
}

void AdjustAlarm(){
    alarm_state = 1;
    alarm_hour = (alarm_value/3600)%13;
    alarm_minute = (alarm_value/60)%61;
    alarm_second = (alarm_value)%61;
}

void CancelAlarm(){
    fire_alarm = 0;
    alarm_state = 0;
    alarm_count = 0;
}

void buzzer(){
    SET_BIT(buzzer_port, buzzer_bit);
    _delay_ms(200);
    RESET_BIT(buzzer_port, buzzer_bit);
    _delay_ms(200);
}

void UpdateTime(){
    time = time%(86400);
    hour = (time/3600)%12 + 1;
	if((time/3600)%13==12)
		toggle(am_pm_t);
    minute = (time/60)%60;
    second = time%60;
}

void UpdateDate(){
    days = (days + time/86400)%361;
    year = (days/361)+1;
    month = ((days/12)+1);
    day = days%31;
}

void UpdateAlarm(){
	alarm_value = alarm_value%(86400);
	alarm_hour = (alarm_value/3600)%12 + 1;
	if((alarm_value/3600)%13==12)
	toggle(am_pm_a);
	alarm_minute = (alarm_value/60)%60;
	alarm_second = alarm_value%60;
}

void UpdateTemperature(){
	// temperature_c = adc_read(temp_bit);
	//ADC Start Conversion
	ADCSRA |=(1<<ADSC);
	// wait for conversion to complete, ADSC becomes ’0? again, till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	temperature_c= ADC/500 ;
}

void UpdateLCD(){
    if(mode == 0){
	    //display year,month,day,temperature_c,alarm_state,hour,minute,second,am/pm
		DisplayMode0();
    }
    else if(mode == 1){
	    //display alarm_hour,alarm_minute,alarm_second,temperature_c
        DisplayMode1();
    }
}

ISR(mode_vect){
    //mode button is clicked
    //Close the alarm if it is fired
    if(fire_alarm == 1){
	    CancelAlarm();
    }
    mode = (mode+1)%n_modes;
    toggle_count = 0;
}

ISR(toggle_vect){
    //toggle button is clicked
    //Close the alarm if it is fired
    if(fire_alarm == 1){
	    CancelAlarm();
    }
    switch(mode){
        case 0 : n_toggles = 8;break;
        case 1 : n_toggles = 6;break;
    }

    toggle_count = (toggle_count+1)%n_toggles;
}

ISR(up_vect){
    //up button is clicked
    //Close the alarm if it is fired
    if(fire_alarm == 1){
	    CancelAlarm();
    }
    if(mode == 0 ){
        switch(toggle_count){
            case 0 : ;break;
            case 1 : time += 3600; break;
            case 2 : time += 60; break;
            case 3 : time++; break;
            case 4 : am_pm_t = toggle(am_pm_t); break;
            case 5 : days += 1; break;
            case 6 : days += 30; break;
            case 7 : days += 360; break;
        }
        UpdateDate();
        UpdateTime();
    }
    else if(mode == 1){//alarm mode
        switch(toggle_count){
            case 0 : ;break;
            case 1 : alarm_value += 3600; break;
            case 2 : alarm_value += 60; break;
            case 3 : alarm_value++; break;
            case 4 : am_pm_a = toggle(am_pm_a); break;
            case 5 : alarm_state = toggle(alarm_state); break;;
        }
        if(alarm_state == 0)
            CancelAlarm();
        else
            AdjustAlarm();
		UpdateAlarm();
    }
    UpdateLCD();
}

ISR(down_vect){
    //down button is clicked
    //Close the alarm if it is fired
    if(fire_alarm == 1){
	    CancelAlarm();
    }
    if(mode == 0 ){
        switch(toggle_count){
            case 0 : ;
            case 1 : time -= 3600; break;
            case 2 : time -= 60; break;
            case 3 : time--; break;
            case 4 : am_pm_t = toggle(am_pm_t); break;
            case 5 : days -= 1; break;
            case 6 : days -= 30; break;
            case 7 : days -= 360; break;
        }
        UpdateDate();
        UpdateTime();
    }
    else if(mode == 1){//alarm mode
        switch(toggle_count){
            case 0 : break;
            case 1 : alarm_value -= 3600; break;
            case 2 : alarm_value -= 60; break;
            case 3 : alarm_value--; break;
            case 4 : am_pm_a = toggle(am_pm_a); break;
            case 5 : alarm_state = toggle(alarm_state); break;
        }
        if(alarm_state == 0)
            CancelAlarm();
        else
            AdjustAlarm();
        UpdateAlarm();
    }
    UpdateLCD();
}

ISR(timer_vect){
    //every 1 second
    time++;
    UpdateDate();
    UpdateTemperature();
    UpdateTime();
	
    //check for the alarm
    if(alarm_state == 1){
		if(alarm_value == alarm_count)
			fire_alarm = 1;
		else
			alarm_count++;
	}
	
	UpdateLCD();
}

int main(void)
{
	CTC_mode();
	ADC_intialzation();
	Lcd4_Init();
	set_as_input(3,DDRC);
	set_as_input(4,DDRC);
	set_as_input(4,DDRC);
	set_as_input(1,DDRD);
	set_as_input(2,DDRD);
	set_as_input(3,DDRD);
	PCICR |= (1<<PCIE1)|(1<<PCIE2);
	PCMSK1 |= (1<<PCINT11); //PCINT11 
	PCMSK2 |= (1<<PCINT17); //PCINT17
	EICRA |= (1<<ISC01)|(1<<ISC11); //The falling edge of INT1 and INT0
	EIMSK |= (1<<INT0)|(1<<INT1);
	sei();
	
    while(1){
		
        if(fire_alarm == 1)
            buzzer();
        if(READ_BIT(ok_port, ok_bit) == 0){
            // button is clicked
			while(READ_BIT(ok_port, ok_bit) == 0)
				;
            //Close the alarm if it is fired
            if(fire_alarm == 1){
                CancelAlarm();
            }
            switch(mode){
                case 0 : toggle_count = 0; break;
                case 1 : mode = 0, toggle_count = 0; break;
            }
            UpdateLCD();
        }
		
    }
    return 0;
	
}