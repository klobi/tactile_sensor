/*
 * timer.h
 *
 *  Created on: 01.04.2015
 *      Author: Klobi
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>


double timer_16_read_ms(void);
void start_timer16(void);
void init_timer(void);
void timer_16_write( unsigned int i );
#endif /* TIMER_H_ */
