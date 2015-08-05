/*
 * timer.c
 *
 *  Created on: 01.04.2015
 *      Author: Klobi
 */

#include "timer.h"

volatile uint8_t timer_overflow = 0;

void timer_16_write( unsigned int i )
{
	cli();
	timer_overflow = 0;
	TCNT3 = i;
	sei();
}

double timer_16_read_ms(void)
{
	unsigned int timer;
	double time = 0;
	double ms;

	uint8_t overflow;
	cli();
	timer = TCNT3;
	overflow = timer_overflow;
	sei();

	ms = (1.0 / ((F_CPU / 64) / 1000));
	time = (double)timer * ms + ms * timer_overflow * 0xffff;
	printf("Time: %fms\n",time);
	return time;
}

void start_timer16()
{
	timer_16_write(0);
}


void init_timer(void)
{
	TCCR3B |= (1 << CS31) | (1 << CS30);
	TIMSK3 |= (1<<TOIE3);
}

ISR(TIMER3_OVF_vect)
{
	timer_overflow++;
}
