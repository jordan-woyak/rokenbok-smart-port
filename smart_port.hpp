
#pragma once

#include "arduino_gpio.hpp"

// TODO: make a class?
// TODO: data pins default state?
// TODO: turn off pull-ups when switching data pins to input?
// TODO: ignore first frame end (it might be missing starting bits)

#define SMART_PORT_USE_INTERRUPT

namespace smart_port_details
{

enum : uint8_t
{
	input_enable_pin = 5, // active low
	output_load_pin = 4, // active low
	accio_pin = 3, // active low
	accio2_pin = 2, // active low
};

#if defined(SMART_PORT_USE_INTERRUPT)
volatile bool frame_end = false;
}

// possibly doable without interrupt?
ISR (INT0_vect)
{
	using namespace smart_port_details;
	
	// seems to be fine up to 500us
	//_delay_us(300);
	digital_write(accio_pin, true);
	
	frame_end = true;
#endif
}

class smart_port
{
private:

uint8_t data_pins[8] =
{
	6, 7, 8, 9, 10, 11, 12, 13
};

//}

public:

void init()
{
	using namespace smart_port_details;
	
	// input enable (active low)
	digital_write(input_enable_pin, true);
	enable_output(input_enable_pin, true);
	
	// output load (active low)
	digital_write(output_load_pin, true);
	enable_output(output_load_pin, true);
	
	// accio output (active low)
	digital_write(accio_pin, false);
	enable_output(accio_pin, true);
	
	// probably default
	digital_write(accio2_pin, false);
	enable_output(accio2_pin, false);
	
#if defined(SMART_PORT_USE_INTERRUPT)
	// enable interrupts (should this be elsewhere?)
	sei();
	
	// enable INT0 falling edge
	MCUCR = (1 << ISC01) | (1 << ISC00);
	EIMSK = (1 << INT0);
#endif
}

uint8_t read()
{
	using namespace smart_port_details;
	
#if defined(SMART_PORT_USE_INTERRUPT)
	// wait for interrupt
	while (!frame_end)
	{}
	frame_end = false;
#else
	// busy wait on pin state
	while (digital_read(accio2_pin))
	{}
	// seems to be fine up to 500us
	//_delay_us(300);
	digital_write(accio_pin, true);
#endif
	
	// read byte
	digital_write(input_enable_pin, false);
	
	// seems to need at least 350us
	//_delay_us(500);
	// or just wait for accio2 to go high I think
	while (!digital_read(accio2_pin))
	{}
	
	uint8_t byte = 0;
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], false);
		byte |= digital_read(data_pins[i]) << i;
	}
	
	digital_write(input_enable_pin, true);
	
	return byte;
}

void write(uint8_t const byte)
{
	using namespace smart_port_details;
	
	// prepare output byte
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], true);
		digital_write(data_pins[i], byte & (1 << i));
	}
	digital_write(output_load_pin, false);
	digital_write(output_load_pin, true);
	
	// seems to be fine up to 7ms
	//_delay_ms(4);
	
	// accio low
	digital_write(accio_pin, false);
}

};
