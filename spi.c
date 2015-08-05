/*
 * spi.h
 *
 * Created: 11.03.2015 10:51:28
 * Author: Nicolas Baenisch
 */ 

#include "spi.h"

/*
 * PORTB
 * 	4: IO8
 * 	5: IO9
 * 	*/

void spi_init_master(void)
{
	/* Set MOSI and SCK, all others input */
	DDRB = (1<<DDB2)|(1<<DDB1);
	/* Enable SPI, Master, set clock rate fck/16 */
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPCR = (1<<SPE)|(1<<MSTR);
	/* SS */
	DDRB |= (1<<DDB4);
	PORTB |= (1<<PB4);
	DDRB |= (1<<DDB5);
	PORTB |= (1<<PB5);
}

int_fast16_t spi_chip_select(uint8_t cs)
{
	switch(cs)
	{
		case 0:
			PORTB &= ~(1<<PB5);
		break;

		case 1:
			PORTB &= ~(1<<PB4);
		break;

		case 15:
			PORTB |= (1<<PB4);
			PORTB |= (1<<PB5);
		break;

		default:
			return 0;
	}
	return 1;
}

unsigned char spi_read_byte(volatile unsigned char cData)
{
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return Data Register */
	return SPDR;
}

void spi_transaction(unsigned char cs, unsigned char * data, unsigned char length)
{
	int i=0;
	/* pointer to the currently processed byte */
	unsigned char* current_data = NULL;
	
	/* No data space */
	if(length==0 || data==NULL)
	return;
	
	/* Select the peripheral device to be accessed */
	spi_chip_select(cs);
	/* Start transmission */
	for(i=0; i<length; i++)
	{
		/* next byte for processing */
		current_data = (data + i);
		/* do the actual transaction*/
		*current_data = spi_read_byte(*current_data);
	}
	
	/* deselect the peripheral device to be accessed */
	spi_chip_select(15);
}

ad7490_value ad7490_read(ad7490* device, unsigned char channel)
{
	/* 1000 0011 0101 0000
	* WRITE = yes
	* SEQ = 0
	* ADD3-ADD0 = 0
	* PM1 = PM0 = 1 (normal mode)
	* SHADOW = 0
	* WEAK/TRI = 1 (weak)
	* RANGE = 0 (0-5V)
	* CODING = 1 (straight binary)
	* four zeros filling
	*/
	unsigned char control[2];
	control[0] = 0x83;
	control[1] = 0x50;
	
	unsigned short raw_value;
	
	ad7490_value value;
	value.channel = channel;
	/* Store the device id */
	value.device = device->chip_select;
	
	/* Add channel request to 'control' */
	control[0] |= channel << 2;
	
	/* Data from wrong channel */
	spi_transaction(device->chip_select, (void*)&control, 2);
	
	control[0] = 0x03;
	control[1] = 0x50;
	control[0] |= channel << 2;
	spi_transaction(device->chip_select, (void*)&control, 2);
	
	/* Get the raw data */
	raw_value = (control[0] << 8) | control[1];
	/* Four first bits in raw data are the address number */
	value.channel = (raw_value >> 12);
	/* Remove the address from the raw value to get the value */
	value.value = raw_value & 0xFFF;

	//printf("ADC Raw: 0x%04x \n",raw_value);
	//printf("ADC Channel: %i \n", value.channel);
	//printf("ADC Value: 0x%03x \n", value.value);

	return value;
}

int spi_init_ad7490(ad7490* device)
{
	unsigned short dummy = 0xFFFF;
	unsigned char i;
	ad7490_value ad7490_temp;
	
	spi_transaction(device->chip_select, (void*)&dummy, 2);
	spi_transaction(device->chip_select, (void*)&dummy, 2);
	
	for(i=0; i<16; i++)
	{
		ad7490_temp = ad7490_read(device, i);


		if(ad7490_temp.channel != i)
		{
			return 0;
		}
	}
	return 1;
}

void spi_init_seq_mode(ad7490* device, unsigned short shadow)
{
	/* 1000 0011 1101 0000
	* WRITE = yes
	* SEQ = 0
	* ADD3-ADD0 = 0
	* PM1 = PM0 = 1 (normal mode)
	* SHADOW = 1
	* WEAK/TRI = 1 (weak)
	* RANGE = 0 (0-5V)
	* CODING = 1 (straight binary)
	* four zeros filling
	*/
	unsigned char control[2];
	unsigned char shadow_bytes[2];
	control[0] = 0x83;
	control[1] = 0xD0;
	shadow_bytes[0] = shadow >> 8;
	shadow_bytes[1] = shadow & 0xFF;

	spi_transaction(device->chip_select, (void*)&control, 2);
	spi_transaction(device->chip_select, (void*)&shadow_bytes, 2);
}

uint8_t spi_seq_read(ad7490* device, ad7490_value* value)
{
	/* 0000 0011 1101 0000
	* WRITE = no
	* SEQ = 0
	* ADD3-ADD0 = 0
	* PM1 = PM0 = 1 (normal mode)
	* SHADOW = 1
	* WEAK/TRI = 1 (weak)
	* RANGE = 0 (0-5V)
	* CODING = 1 (straight binary)
	* four zeros filling
	*/
	unsigned char control[2];
	control[0] = 0x03;
	control[1] = 0xD0;

	value->device = device->chip_select;
	unsigned short raw_value;

	spi_transaction(device->chip_select, (void*)&control, 2);
	
	/* Get the raw data */
	raw_value = (control[0] << 8) | control[1];
	/* Four first bits in raw data are the address number */
	value->channel = (raw_value >> 12);
	/* Remove the address from the raw value to get the value */
	value->value = raw_value & 0xFFF;

	return 0;
}

uint8_t spi_init_read_all(ad7490* device)
{
	/* Control Register:
	*   11    10    9     8     7     6    5    4      3        2       1       0
	* WRITE  SEQ  ADD3  ADD2  ADD1  ADD0  PM1  PM0  SHADOW  WEAK/TRI  RANGE  CODING
	* 1111 1111 1101 0000
	* WRITE = yes
	* SEQ = 1
	* ADD3-ADD0 = 1111
	* PM1 = PM0 = 1 (normal mode)
	* SHADOW = 1
	* WEAK/TRI = 1 (weak)
	* RANGE = 0 (0-5V)
	* CODING = 1 (straight binary)
	* four zeros filling
	*/
	unsigned char control[2];
	control[0] = 0xFF;
	control[1] = 0xD0;
	spi_transaction(device->chip_select, (void*)&control, 2);

	return 0;
}

uint8_t spi_read_all(ad7490* device, uint16_t* values)
{
	int i;
	uint8_t t = 0;

	for(i=0;i<16;i++)
	{
		spi_chip_select(device->chip_select);
		SPDR = t;
		while(!(SPSR & (1<<SPIF)));
		values[i] = SPDR << 8;

		SPDR = t;
		while(!(SPSR & (1<<SPIF)));
		values[i] |= SPDR;
		spi_chip_select(15);
	}
	return 0;
}

void spi_print(ad7490_value * value)
{
	int i = 0;
	double e = 5.0;
	e = 5 / 4096.0;

	for(i=0; i<16; i++)
	{
		printf("ADC%i: Channel: %02i Value: 0x%03x Volt: %1.2f\n", value[i].device, value[i].channel, value[i].value, e*value[i].value);
	}
}

int spi_init_devices(ad7490 * devices,  unsigned char length)
{
	int i=0;
	for (i=0; i<length;i++)
	{
		if(spi_init_ad7490((void*)&devices[i]))
		{
			spi_init_read_all((void*)&devices[i]);
		}
		else
		{
			printf("ADC%i initialization failed!\n", devices[i].chip_select);
			return 1;
		}
	}
	return 0;
}
