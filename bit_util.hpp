
#pragma once

template <typename T>
void set_bit(T& lhs, uint8_t bit, bool rhs)
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
