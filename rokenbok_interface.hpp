
#pragma once

#include "thumbpad.hpp"
#include "bit_util.hpp"

// TODO: debug all bytes of edit_select

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
			else if (out_command::bcast_tpads == read_cmd)
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
		
		io.write((1 << 0) | (1 << 2) | (1 << 4)); // <atrib value>
		input_assert(out_command::readattrib, io.read());
		
		io.write(0x01); // <no sel timeout value>
		input_assert(out_command::readnoseltimeout, io.read());
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
		
		write_command(io, in_command::nullcmd);
		input_assert(out_command::bcast_end, io.read());
	}
	
	template <typename T>
	void process_edit_tpads(T&& io)
	{
		write_command(io, in_command::vfyedit);
		
		bool dpad_up = false;
		
		for (uint8_t byte = 0; byte != tpad_byte_count; ++byte)
		{
			uint8_t tpad_data = io.read();
			
			// OK, having more than 3 bits set in any bytes causes dsync
			// even when the bits were received as set
			// TODO: why?
			
			// TODO: A & B & !(X | Y | R) causes a desync?
			
			//debug_byte(tpad_data);
			/*
			if (thumbpad_button::dpad_up == byte)
				dpad_up = get_bit(tpad_data, 0);
			
			else if (thumbpad_button::dpad_left == byte)
				set_bit(dpad_up, 0, true);
			
			//if (0 == byte)
			//	for (int i = 0; i != 8; ++i)
			//		set_bit(tpad_data, i, false);
			*/
			//if (byte != 16)
			//	for (int i = 0; i != 8; ++i)
			//		set_bit(tpad_data, i, false);
				//for (int i = 4; i != 8; ++i)
					//set_bit(tpad_data, i, false);
					
			//tpad_data &= 0x0f;
				
			if (thumbpad_button::dpad_up == byte)
			{
				dpad_up = get_bit(tpad_data, 0);
				tpad_data = 0;
			}
			else if (thumbpad_button::dpad_down == byte)
			{
				set_bit(tpad_data, 0, dpad_up);
			}			
				//tpad_data &= 0xf0;
			//else
			tpad_data &= 0x01;
				
			//set_bit(tpad_data, 0, false);
				
			//if (byte == 16)
			//	tpad_data = 0x85;
				
			io.write(tpad_data);
		}
		
		// TODO: having more than 5 (or 4?) bits in mask set causes desync
		
		uint8_t priority_mask = io.read();
		//debug_byte(priority_mask);
		io.write(0x01);
		
		auto rd = io.read();
		//debug_byte(rd);
		input_assert(out_command::bcast_end, rd);
	}
	
	static const uint8_t tpad_count = 8;
	
	template <typename T>
	void process_bcast_select(T&& io)
	{
		for (uint8_t byte = 0; byte != tpad_count; ++byte)
		{
			write_command(io, in_command::nullcmd);
			uint8_t selection = io.read();
		}
		
		write_command(io, in_command::nullcmd);
		uint8_t timer_value = io.read();
		
		write_command(io, in_command::nullcmd);
		input_assert(out_command::bcast_end, io.read());
	}
	
	template <typename T>
	void process_edit_select(T&& io)
	{
		write_command(io, in_command::vfyedit);
		
		uint8_t const force_selection = 3;
		for (uint8_t byte = 0; byte != tpad_count; ++byte)
		{
			uint8_t selection = io.read();
			io.write(force_selection);
		}
		
		uint8_t timer_value = io.read();
		// TODO: changing this seems to have no effect
		// it's probably ignored (should be nullcmd)
		io.write(timer_value);
		
		input_assert(out_command::bcast_end, io.read());
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
		//if ((uint8_t)cmd != val)
			//...
	}
	
	static const uint8_t thumbpad_button_count = 17;
	
	uint8_t thumbpad_button_state[thumbpad_button_count];
	
	data_handler* active_handler;
	
	// TODO: var names
	//main_handler main;
	//sync_check_handler sync;
};
