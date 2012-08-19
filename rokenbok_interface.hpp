
#pragma once

#include "thumbpad.hpp"
#include "bit_util.hpp"

// output: (from control deck)
enum class out_command : uint8_t
{
	nullcmd = 0x00,

	bcast_tpads = 0xc0,
	bcast_select = 0xc1,
	bcast_end = 0xc2,
	edit_tpads = 0xc3,
	edit_select = 0xc4,
	edit_end = 0xc5,
	presync = 0xc6,
	sync = 0xc7,
	readattrib = 0xc8,
	noins = 0xc9, //?
	askins = 0xca, //?
	readreply = 0xcb, //?
	readnoseltimeout = 0xcc,
	noradiopkt = 0xcd, //?
	haveradiopkt = 0xce, //?
};

// input: (to control deck)
enum class in_command : uint8_t
{
	nullcmd = 0x00,

	vfyedit = 0x80,
	sync = 0x81,
	noins = 0x82, //?
	waitins = 0x83, //?
};

class data_handler
{
public:
	virtual void handle_data(uint8_t byte) = 0;
};

class main_handler : public data_handler
{
//public:
	void handle_data(uint8_t byte) override
	{
		
	}
};

class sync_check_handler : public data_handler
{
//public:
	void handle_data(uint8_t byte) override
	{
		
	}
};

class rokenbok_interface
{
public:
	rokenbok_interface()
		: thumbpad_button_state{}
		, active_handler{}
	{
		//active_handler = &main
	}
	
	void input_data(uint8_t byte)
	{
		//return;
		//active_handler->handle_data(byte);
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
	
	data_handler* active_handler;
	
	// TODO: var names
	//main_handler main;
	//sync_check_handler sync;
};
