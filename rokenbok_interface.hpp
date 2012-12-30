
#pragma once

#include "thumbpad.hpp"
#include "bit_util.hpp"

// TODO: debug all bytes of edit_select
// TODO: debug all bytes of bcast_tpads?

/*__attribute__((always_inline))*/ void debug_string(const char* str)
{
	//USART_puts(str);
}

void debug_byte(uint8_t byte)
{
	char str[3] = "";
	sprintf(str, "%02x", byte);
	USART_puts(str);
}

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

enum attrib_bit : uint8_t
{
	sync = 0, // should be 1 for active accessory
	notsync = 1, // should be 0 for active accessory (why?)
	edit_tpads = 2, // request edit_tpads command
	edit_select = 3, //request edit_select command
	suppress_select = 4, //disable select buttons (useful in combination with edit_select)
	pkt_inject = 5,
	suppress_radio = 6, // disable radio
	full_rf_null = 7, //
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
		, is_synced{}
	{
		//active_handler = &main
	}
	
	// TODO: better fuction name
	template <typename T>
	void process(T&& io)
	{
		//io_adapter<T> io(_io);
		
		while (true)
		{
			auto const read_cmd = read_command(io);
			
			if (out_command::presync == read_cmd)
			{
				debug_string("presync");
				process_presync(io);
			}
			else// if (is_synced)
			{	
				if (out_command::bcast_tpads == read_cmd)
				{
					debug_string("bcast_tpads");
					//debug_byte(0x00);
					process_bcast_tpads(io);
				}
				else if (out_command::edit_tpads == read_cmd)
				{
					debug_string("edit_tpads");
					process_edit_tpads(io);
				}
				else if (out_command::bcast_select == read_cmd)
				{
					debug_string("bcast_select");
					process_bcast_select(io);
				}
				else if (out_command::edit_select == read_cmd)
				{
					debug_string("edit_select");
					process_edit_select(io);
				}
				else if (out_command::haveradiopkt == read_cmd)
				{
					debug_string("haveradiopkt");
					process_haveradiopkt(io);
				}
				else if (out_command::nullcmd == read_cmd)
				{
					debug_string("nullcmd");
				}
				else
				{
					debug_byte((uint8_t)read_cmd);
					is_synced = false;
				}
			}
			
			write_command(io, in_command::nullcmd);
		}
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
	/*
	template <typename T>
	class io_adapter
	{
	public:
		io_adapter(T& _obj)
			: obj(_obj)
		{}
		
		out_command read()
		{
			return (uint8_t)obj.read();
		}
		
		void write(in_command val)
		{
			return obj.write((uint8_t)val);
		}
		
	private:
		T& obj;
	};
	*/
	
	template <typename T>
	void process_presync(T&& io)
	{
		write_command(io, in_command::sync);
		input_assert(out_command::sync, io.read());
		
		io.write((1 << attrib_bit::sync)
			//| (1 << attrib_bit::suppress_select)
			| (1 << attrib_bit::edit_tpads)
			| (1 << attrib_bit::edit_select)
		);
		// TODO: not actually receiving readattrib
		// actually getting 0x50 or 0xae (wtf?)
		input_assert(out_command::readattrib, io.read());
		
		io.write(0x00); // <no sel timeout value>
		input_assert(out_command::readnoseltimeout, io.read());
		
		is_synced = true;
	}
	
	static const uint8_t tpad_byte_count = 17;
	
	template <typename T>
	void process_bcast_tpads(T&& io)
	{
		for (uint8_t byte = 0; byte != tpad_byte_count; ++byte)
		{
			write_command(io, in_command::nullcmd);
			uint8_t tpad_data = io.read();
		}
		
		write_command(io, in_command::nullcmd);
		uint8_t priority_mask = io.read();
		
		int i = 0;
		while (true)
		{
			++i;
			write_command(io, in_command::nullcmd);
			auto end = /*input_assert(out_command::bcast_end, */io.read();//);
			
			if (end == (uint8_t)out_command::bcast_end)
				break;
		}
		//debug_byte(i);
	}
	
	template <typename T>
	void process_edit_tpads(T&& io)
	{
		fill_pad_data(thumbpad_button_state);
		
		write_command(io, in_command::vfyedit);
		
		bool dpad_up = false;
		
		//debug_byte(0x00);
		
		for (uint8_t byte = 0; byte != tpad_byte_count; ++byte)
		{
			uint8_t tpad_data = io.read();
			
			//debug_byte(tpad_data);
			
			// OK, having more than 3 bits set in any bytes causes dsync
			// even when the bits were received as set
			// TODO: why? actually, is that still an issue?
			
			// sometimes some of the 4 high bits (virtual pads) are set
			// TODO: why?
			
			//if (thumbpad_button::dpad_up == byte)
				//tpad_data &= ~0x01;
			
			tpad_data &= 0x0f;
			tpad_data |= thumbpad_button_state[byte];
				
			//if (byte != 16)
			//tpad_data &= 0x0f;
				
			io.write(tpad_data);
		}
		
		// TODO: having more than 5 (or 4? or 6?) bits in mask set causes desync
		// zero still allows base to see changes :/
		
		uint8_t priority_mask = io.read();
		//debug_byte(priority_mask);
		io.write(0x01);
		
		auto rd = io.read();
		//debug_byte(rd);
		input_assert(out_command::edit_end, rd);
	}
	
	static const uint8_t tpad_count = 8;
	
	template <typename T>
	void process_bcast_select(T&& io)
	{
		// I think 0xf == no selection
		
		//debug_byte(0x00);
		for (uint8_t byte = 0; byte != tpad_count; ++byte)
		{
			write_command(io, in_command::nullcmd);
			uint8_t selection = io.read();
			//debug_byte(selection);
		}
		
		write_command(io, in_command::nullcmd);
		uint8_t const timer_value = io.read();
		
		while (true)
		{
			write_command(io, in_command::nullcmd);
			auto end = /*input_assert(out_command::bcast_end, */io.read();//);
			
			if (end == (uint8_t)out_command::bcast_end)
				break;
		}
	}
	
	template <typename T>
	void process_edit_select(T&& io)
	{
		write_command(io, in_command::vfyedit);
		
		uint8_t const force_selection = 1;
		uint8_t const force_controller = 4;
		
		for (uint8_t byte = 0; byte != tpad_count; ++byte)
		{
			uint8_t selection = io.read();
			if (force_controller == byte)
				selection = force_selection;
			io.write(selection);
		}
		
		uint8_t const timer_value = io.read();
		// TODO: changing this seems to have no effect
		// it's probably ignored (should be nullcmd)
		io.write(timer_value);
		
		input_assert(out_command::edit_end, io.read());
	}
	
	template <typename T>
	void process_haveradiopkt(T&& io)
	{
		//debug_byte(0xbe);
		uint8_t radiopkt = ~0x00;
		
		// seems to only work with 2
		for (int i = 0; i != 2; ++i)//radiopkt != 0x00)
		{
			write_command(io, in_command::nullcmd);
			radiopkt = io.read();
			//debug_byte(radiopkt);
		}
		
		//debug_byte(0xbd);
	}
	
	template <typename T>
	static out_command read_command(T&& io)
	{
		return (out_command)io.read();
	}
	
	template <typename T>
	static void write_command(T&& io, in_command cmd)
	{
		io.write((uint8_t)cmd);
	}
	
	void input_assert(out_command cmd, uint8_t val)
	{
		// TODO: also need to break out of presync I think
		
		if ((uint8_t)cmd != val)
		{
			is_synced = false;
			debug_byte(0xcc);
			debug_byte((uint8_t)cmd);
			debug_byte(val);
		}
	}
	
	uint8_t thumbpad_button_state[thumbpad_button_count];
	
	data_handler* active_handler;
	
	bool is_synced;
	
	// TODO: var names
	//main_handler main;
	//sync_check_handler sync;
};
