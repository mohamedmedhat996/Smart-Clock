/*
 * adc.c
 *
 * Created: 6/1/2018 4:17:59 PM
 *  Author: yosef
 */ 


#ifndef ADC_H_
#define ADC_H_

void adc_init(unsigned char flag_8_16);
unsigned int adc_read(unsigned char ch);
unsigned char adc_read_8_bit(unsigned char ch);

#endif /* ADC_H_ */

