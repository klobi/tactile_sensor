/*
 * uart.h
 *
 * Created: 11.03.2015 10:51:28
 * Author: Nicolas Baenisch
 */ 

/* This contains definitions for all the registers locations and some other things,
   must always be included */
#include <avr/io.h>
#include <stdio.h>

#ifndef UART_H_
#define UART_H_

/* F_CPU tells the compiler that our crystal is an 16Mhz one so it can generate an accurate delay,
   must be declared above delay so delay knows what is the value of F_CPU */
#define F_CPU 16000000UL

#define BAUDRATE 115200
#define BAUD_PRESCALLER (((F_CPU/(16UL*BAUDRATE))))



char uart_rx(FILE *stream);

void uart_tx(char data, FILE *stream);

void uart_init(void);

#endif /* UART_H_ */
