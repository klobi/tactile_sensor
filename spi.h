/*
 * spi.h
 *
 * Created: 11.03.2015 10:51:28
 * Author: Nicolas Baenisch
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SPI_H_
#define SPI_H_

typedef struct {
	unsigned char chip_select;
} ad7490;

typedef struct {
	unsigned char device;
	unsigned char channel;
	uint16_t value;
} ad7490_value;

void spi_init_master(void);

int_fast16_t spi_chip_select(uint8_t cs);

unsigned char spi_read_byte(volatile unsigned char cData);

void spi_transaction(unsigned char cs, unsigned char * data, unsigned char length);

ad7490_value ad7490_read(ad7490* device, unsigned char channel);

int spi_init_ad7490(ad7490* device);

void spi_init_seq_mode(ad7490* device, unsigned short shadow);

uint8_t spi_seq_read(ad7490* device, ad7490_value* value);

void spi_print(ad7490_value * value);

int spi_init_devices(ad7490 * devices,  unsigned char length);

uint8_t spi_init_read_all(ad7490* device);
uint8_t spi_read_all(ad7490* device, uint16_t* values);
uint8_t read_adc_data(ad7490* device, ad7490_value* value);


#endif /* SPI_H_ */
