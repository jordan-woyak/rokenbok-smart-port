
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <util/setbaud.h>

//#include <stdio.h>
#include <stdlib.h>

#include "usart.hpp"
#include "rokenbok_interface.hpp"
#include "smart_port.hpp"

void loop();

rokenbok_interface rokinf;

int main()
{
	//free(malloc(5));
	
	USART_init();
	
	while (true)
		loop();
}

void loop()
{
	uint8_t const read_byte = smart_port::read();
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
	
	smart_port::write(write_byte);
}
