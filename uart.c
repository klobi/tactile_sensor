/*
 * uart.c
 *
 * Created: 11.03.2015 10:51:28
 * Author: Nicolas Baenisch
 */ 

#include "uart.h"

void uart_init(void)
{
	UBRR1H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR1L = (uint8_t)(BAUD_PRESCALLER);
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	UCSR1C = ((1<<UCSZ10)|(1<<UCSZ11));
}

char uart_rx(FILE *stream)
{
	while(!(UCSR1A & (1<<RXC1))); /* Wait until data exists. */
	return UDR1;
}

void uart_tx(char data, FILE *stream)
{
	if( data == '\n' )
		uart_tx( '\r', stream );
	/* Wait for empty transmit buffer */
	while(!(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}
