
#pragma once

#include "thumbpad.hpp"

class rokenbok_interface
{
public:
	rokenbok_interface()
		: thumbpad_button_state{}
	{
	}
	
	void input_data(uint8_t byte)
	{
		
	}
	
	uint8_t output_data()
	{
		return 0;
	}
	
	bool get_thumbpad_button(uint8_t pad, thumbpad_button button) const
	{
		return get_bit(thumbpad_button_state[(int)button], pad);
	}
	
	void set_thumbpad_button(uint8_t pad, thumbpad_button button, bool value)
	{
		set_bit(thumbpad_button_state[(int)button], pad, value);
	}
	
private:
	static const uint8_t thumbpad_button_count = 17;
	
	uint8_t thumbpad_button_state[thumbpad_button_count];
};