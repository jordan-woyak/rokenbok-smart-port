
#pragma once

void USART_init()
{
	UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = ((1 << UCSZ00) | (1 << UCSZ01));
}

void USART_putc(uint8_t data)
{
	while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = data;
}

void USART_puts(char const* str)
{
	while (*str != 0x00)
	{
		USART_putc(*str);
		++str;
	}
	
	//USART_putc('\r');
	USART_putc('\n');
}