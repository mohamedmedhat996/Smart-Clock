/*
 * Smart Clock.c
 *
 * Created: 6/1/2018 7:42:36 PM
<<<<<<< Updated upstream
 */
#define buzzer_bit 0
#define buzzer_port PORTB
#define timer_vect TIMER1_COMPA_vect
#define mode_bit
#define mode_port
#define mode_vect INT0_vect
#define temp_bit
#define temp_port
#define toggle_bit
#define toggle_port
#define toggle_vect INT1_vect
#define ok_bit 1
#define ok_port PINB
#define ok_vect
#define up_bit
#define up_port
#define up_vect PCINT2_vect
#define down_bit
#define down_port
#define down_vect PCINT1_vect
#define LCD_port
#define F_CPU 16000000ul

#define SET_BIT(ADDRESS, BIT) ADDRESS |= (1<<BIT)
#define RESET_BIT(ADDRESS, BIT) ADDRESS &= ~(1<<BIT)
#define TOGGLE_BIT(ADDRESS, BIT) ADDRESS ^= (1<<BIT)
#define READ_BIT(ADDRESS, BIT) ((ADDRESS & (1<<BIT))>>BIT)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "adc.h"

unsigned short am_pm = 0;
unsigned short mode = 0;
unsigned short n_modes = 2;

unsigned short n_toggles = 0;
unsigned short toggle_count = 0;

unsigned short temperature_c = 0;

unsigned long time = 0;
unsigned short hour = 1;
unsigned short minute = 0;
unsigned short second = 0;

unsigned short days = 0;
unsigned short day = 1;
unsigned short year = 1;
unsigned short month = 1;

unsigned long alarm_count = 0;
unsigned long alarm_value = 0;
unsigned short alarm_state = 0;
unsigned short fire_alarm = 0;
unsigned short alarm_hour = 0;
unsigned short alarm_minute = 0;
unsigned short alarm_second = 0;

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
	TCCR1B |= (1<<WGM12)|(1<<CS12);

	//OCRn =  [ (clock_speed / Prescaler_value) * Desired_time_in_Seconds ] - 1
	//OCRn =  [ (16 M / 256) * 1 ] - 1 = 62499 = F423 Hex
	// Set the value that you want to count to
	OCR1AH = 0xF4;
	OCR1AL =0x23;

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
    minute = (time/60)%61;
    second = time%61;
}

void UpdateDate(){
    days = (days + time/(86400-1))%361;
    year = days/361;
    month = (days/13)%31;
    day = days%31;
}

void UpdateTemperature(){
	// temperature_c = adc_read(temp_bit);
	//ADC Start Conversion
	ADCSRA |=(1<<ADSC);
	// wait for conversion to complete, ADSC becomes ’0? again, till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	temperature_c= ADC ;
}

void UpdateLCD(){
    if(mode == 0){
        //display year,month,day,temperature_c,alarm_state,hour,minute,second,am/pm
    }
    else if(mode == 1){
        //display alarm_hour,alarm_minute,alarm_second,temperature_c
    }
}

ISR(mode_vect){
    //mode button is clicked
    //close the alarm if it is fired
    if(fire_alarm == 1){
        CancelAlarm();
        UpdateLCD();
    }
    mode = (mode+1)%n_modes;
    toggle_count = 0;
    _delay_ms(500);
}

ISR(toggle_vect){
    //toggle button is clicked
    //close the alarm if it is fired
    if(fire_alarm == 1){
        CancelAlarm();
        UpdateLCD();
    }
    switch(mode){
        case 0 : n_toggles = 8;break;
        case 1 : n_toggles = 6;break;
    }

    toggle_count = (toggle_count+1)%n_toggles;
    _delay_ms(500);
}

ISR(up_vect){
    //up button is clicked
    //close the alarm if it is fired
    if(fire_alarm == 1){
        CancelAlarm();
        UpdateLCD();
    }
    if(mode == 0 ){
        switch(toggle_count){
            case 0 : ;break;
            case 1 : time += 3600; break;
            case 2 : time += 60; break;
            case 3 : time++; break;
            case 4 : toggle(am_pm); break;
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
            case 4 : am_pm = toggle(am_pm); break;
            case 5 : alarm_state = toggle(alarm_state); break;;
        }
        if(alarm_state == 0)
            CancelAlarm();
        else
            AdjustAlarm();
    }
    UpdateLCD();
    _delay_ms(500);
}

ISR(down_vect){
    //down button is clicked
    //close the alarm if it is fired
    if(fire_alarm == 1){
        CancelAlarm();
        UpdateLCD();
    }
    if(mode == 0 ){
        switch(toggle_count){
            case 0 : ;
            case 1 : time -= 3600; break;
            case 2 : time -= 60; break;
            case 3 : time--; break;
            case 4 : toggle(am_pm); break;
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
            case 4 : am_pm = toggle(am_pm); break;
            case 5 : alarm_state = toggle(alarm_state); break;
        }
        if(alarm_state == 0)
            CancelAlarm();
        else
            AdjustAlarm();
    }
    UpdateLCD();
    _delay_ms(500);
}

/*
ISR(ok_vect){
    //ok button is clicked
    //close the alarm if it is fired
    if(fire_alarm == 1){
        CancelAlarm();
        UpdateLCD();
    }
    switch(mode){
        case 0 : toggle_count = 0;
        case 1 : mode = 0;
    }
    UpdateLCD();
    _delay_ms(500);
}
*/

ISR(timer_vect){
    //every 1 second
    time++;
    UpdateDate();
    UpdateTime();
    UpdateTemperature();
    //check for the alarm
    if(alarm_state == 1 && alarm_value == alarm_count)
        fire_alarm = 1;
    else if(alarm_state == 1)
        alarm_count++;

    //check for mode
    if(mode == 0){
        //clock mode
        //update LCD
        //LCD_SendString();
    }
    else if(mode == 1){
        //alarm mode
        //update LCD
       // LCD_SendString();
    }
}

int main(void)
{
	SET_BIT(PCMSK1,3);   //PCINT11
	SET_BIT(PCMSK2,1);   //PCINT17
	CTC_mode();
	ADC_intialzation();
    while(1){
        if(fire_alarm == 1)
            buzzer();
        if(READ_BIT(ok_port, ok_bit) == 0){
            // button is clicked
            //Close the alarm if it is fired
            if(fire_alarm == 1){
                CancelAlarm();
                UpdateLCD();
            }
            switch(mode){
                case 0 : toggle_count = 0; break;
                case 1 : mode = 0, toggle_count = 0; break;
            }
            UpdateLCD();
            _delay_ms(500);
        }
    }

    return 0;
}