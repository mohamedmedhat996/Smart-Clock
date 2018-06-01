/*
 * timer.h
 *
 * Created: 6/1/2018 4:58:37 PM
 *  Author: yosef
 */ 


#ifndef TIMER_H_
#define TIMER_H_

void normal_mode();
void InitPWM();
void SetPWMOutput(uint8_t duty);
void CTC_mode();

#endif /* TIMER_H_ */