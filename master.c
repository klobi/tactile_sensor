/*
 * Created: 17.07.2014 16:49:48
 *  Author: Nicolas Baenisch
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>

FILE mystdout = FDEV_SETUP_STREAM(uart_tx, NULL,_FDEV_SETUP_WRITE);
FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_rx, _FDEV_SETUP_READ);

#define nop() asm volatile("nop")

void main_init(void)
{
	init_timer();
	uart_init();

	stdout = &mystdout;
	stdin = &mystdin;

	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	spi_init_master();
}

int main(void)
{
	DDRB |= (1<<DDB6);
	PORTB |= (1<<PB6);

	ad7490 devices[2];
	devices[0].chip_select = 0;
	//devices[1].chip_select = 1;

	main_init();

	printf("Hello and Welcome!\n");
	int i = 0;

	if (spi_init_devices((void*)&devices, 1))
	{
		printf("Fail\n");
		exit(1);
	}

	while(1)
	{
		uint16_t values[16];

		start_timer16();
		asm("NOP");
		timer_16_read_ms();

		start_timer16();
		for(i=0; i<40; i++)
		{
			asm("NOP");
		}
		timer_16_read_ms();

		start_timer16();
		spi_read_all((void*)&devices, values);
		timer_16_read_ms();

		start_timer16();
		for(i=0; i<10; i++)
		{
			spi_read_all((void*)&devices, values);
		}
		timer_16_read_ms();

		getchar();
	}
}
