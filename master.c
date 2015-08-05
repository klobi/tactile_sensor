/*
 * GccApplication1.c
 *
 * Created: 17.07.2014 16:49:48
 *  Author: Nicolas Baenisch
 */


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "timer.h"
#include <float.h>
#include <util/delay.h>

FILE mystdout = FDEV_SETUP_STREAM(uart_tx, NULL,_FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_rx, _FDEV_SETUP_READ);

void main_init(void)
{
	init_timer();
	uart_init();

	stdout = &mystdout;
	stdin = &mystdin;
}

int main(void)
{
	main_init();

	printf("Hello and Welcome!\n");
	while(1)
	{
		getchar();
		//Delay_MS(10000);
	}
}

