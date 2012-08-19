
#pragma once

#include "bit_util.hpp"

// TODO: optimize more?

bool digital_read(uint8_t pin)
{
	if (pin < 8)
		return get_bit(PIND, pin);
	else
		return get_bit(PINB, pin - 8);
}

void digital_write(uint8_t pin, bool value)
{
	if (pin < 8)
		set_bit(PORTD, pin, value);
	else
		set_bit(PORTB, pin - 8, value);
}

void enable_output(uint8_t pin, bool value)
{
	if (pin < 8)
		set_bit(DDRD, pin, value);
	else
		set_bit(DDRB, pin - 8, value);
}
