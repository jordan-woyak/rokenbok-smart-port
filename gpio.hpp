
#pragma once

// TODO: optimize more?

template <typename T>
void set_bit(T&& lhs, uint8_t bit, bool rhs)
{
	if (rhs)
		lhs |= (1 << bit);
	else
		lhs &= ~(1 << bit);
}

template <typename T>
bool get_bit(T const& lhs, uint8_t bit)
{
	return 0 != (lhs & (1 << bit));
}

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
		set_bit(DDRB, pin, value);
}