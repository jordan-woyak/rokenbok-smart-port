
#pragma once

#include "thumbpad.hpp"
#include "bit_util.hpp"

__attribute__((always_inline)) void debug_string(const char* str)
{
	//USART_puts(str);
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
				process_bcast_tpads(io);
			}
			else
			{
				write_command(io, in_command::nullcmd);
			}
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
		io.write((1 << 0) | (1 << 4)); // <atrib value>
		input_assert(out_command::readattrib, io.read());
		io.write(0x01); // <no sel timeout value>
		input_assert(out_command::readnoseltimeout, io.read());
		write_command(io, in_command::nullcmd);
	}
	
	template <typename T>
	void process_bcast_tpads(T&& io)
	{/*
		write_command(io, in_command::sync);
		input_assert(out_command::sync, io.read());
		io.write((1 << 0) | (1 << 4)); // <atrib value>
		input_assert(out_command::readattrib, io.read());
		io.write(0x01); // <no sel timeout value>
		input_assert(out_command::readnoseltimeout, io.read());
		*/
		write_command(io, in_command::nullcmd);
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
