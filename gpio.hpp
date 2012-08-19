
#pragma once

// TODO: optimize?

bool digital_read(uint8_t pin)
{
	if (pin < 8)
		return PIND & (1 << pin);
	else
		return PINB & (1 << (pin - 8));
}

void digital_write(uint8_t pin, bool value)
{
	if (pin < 8)
	{
		if (value)
			PORTD |= (1 << pin);
		else
			PORTD &= ~(1 << pin);
	}
	else
	{
		if (value)
			PORTB |= (1 << (pin - 8));
		else
			PORTB &= ~(1 << (pin - 8));
	}
}

void enable_output(uint8_t pin, bool value)
{
	if (pin < 8)
	{
		if (value)
			DDRD |= (1 << pin);
		else
			DDRD &= ~(1 << pin);
	}
	else
	{
		if (value)
			DDRB |= (1 << (pin - 8));
		else
			DDRB &= ~(1 << (pin - 8));
	}
}