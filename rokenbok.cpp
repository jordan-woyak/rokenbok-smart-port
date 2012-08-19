
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <util/setbaud.h>

//#include <stdio.h>
#include <stdlib.h>

#include "gpio.hpp"
#include "usart.hpp"
#include "thumbpad.hpp"
#include "rokenbok_interface.hpp"

volatile bool frame_end = false;

void loop();

enum : uint8_t
{
	input_enable_pin = 5, // active low
	output_load_pin = 4, // active low
	accio_pin = 3, // active low
	accio2_pin = 2, // active low
};

uint8_t data_pins[8] =
{
	6, 7, 8, 9, 10, 11, 12, 13
};

rokenbok_interface rokinf;

int main()
{
	//free(malloc(5));
	
	// everything low by default
	//PORTB = PORTC = PORTD = 0;
	
	// input enable (active low)
	digital_write(input_enable_pin, true);
	enable_output(input_enable_pin, true);
	
	// output load (active low)
	digital_write(output_load_pin, true);
	enable_output(output_load_pin, true);
	
	// accio output (active low)
	digital_write(accio_pin, false);
	enable_output(accio_pin, true);
	
	// probably default
	enable_output(accio2_pin, false);
	
	// TODO: ingore first frame end
	
	// enable interrupts
	sei();
	//GIMSK  |= (1<<INT0);
	
	// TODO: abstract?
	
	// enable INT0 falling edge
	MCUCR = (1 << ISC01) | (1 << ISC00);
	EIMSK = (1 << INT0);
	
	USART_init();
	
	while (true)
		loop();
}

void loop()
{
	// wait for frame end
	while (!frame_end)
	{}
	frame_end = false;
	
	// read byte
	digital_write(input_enable_pin, false);
	
	// seems to need at least 350us
	_delay_us(500);
	
	uint8_t read_byte = 0;
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], false);
		read_byte |= digital_read(data_pins[i]) << i;
	}
	digital_write(input_enable_pin, true);
	
	rokinf.input_data(read_byte);
	
	/*
	char str[3] = "";
	sprintf(str, "%02x", read_byte);
	USART_puts(str);
	*/
	
	// write byte
	uint8_t write_byte = 0x00;
	
	if (0xc6 == read_byte)
	{
		//USART_puts("presync");
		write_byte = 0x81; // sync
	}
	else if (0xc7 == read_byte)
	{
		//USART_puts("sync");
		write_byte = (1 << 0) | (1 << 0); // <atrib value>
	}
	else if (0xc8 == read_byte)
	{
		//USART_puts("readattrib");
		write_byte = 0x01; // <no sel timeout value>
	}
	else if (0xcc == read_byte)
	{
		//USART_puts("readnoseltimeout");
		//write_byte = 0x01;
	}
	else
	{
		//USART_puts(read_byte, HEX); 
	}
	
	// prepare output byte
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], true);
		digital_write(data_pins[i], write_byte & (1 << i));
	}
	digital_write(output_load_pin, false);
	digital_write(output_load_pin, true);
	
	// seems to be fine up to 7ms
	//_delay_ms(4);
	
	// accio low
	digital_write(accio_pin, false);
}

ISR (INT0_vect)
{
	// seems to be fine up to 500us
	//_delay_us(300);
	
	// make accio high
	digital_write(accio_pin, true);
	
	frame_end = true;
}
