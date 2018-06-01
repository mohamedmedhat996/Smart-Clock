/*
 * Smart Clock.c
 *
 * Created: 6/1/2018 7:42:36 PM
 * Author : Mohamed Medhat
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "adc.h"

#define SET_BIT(ADDRESS, BIT) ADDRESS |= (1<<BIT)
#define RESET_BIT(ADDRESS, BIT) ADDRESS &= ~(1<<BIT)
#define TOGGLE_BIT(ADDRESS, BIT) ADDRESS ^= (1<<BIT)
#define READ_BIT(ADDRESS, BIT) ((ADDRESS & (1<<BIT))>>BIT)


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		
    }
}

