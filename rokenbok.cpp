
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 115200
#include <util/setbaud.h>

volatile bool frame_end = false;

void loop();

bool digital_read(uint8_t pin)
{
	if (pin < 8)
		return PORTD & (1 << pin);
	else
		return PORTB & (1 << (pin - 8));
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

enum : uint8_t
{
	input_enable_pin = 5, // active low
	output_load_pin = 5, // active low
	accio_pin = 3, // active low
	accio2_pin = 2, // active low
};

uint8_t data_pins[8] =
{
	6, 7, 8, 9, 10, 11, 12, 13
};

int main()
{	
	// everything low by default
	//PORTB = PORTC = PORTD = 0;
	
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
	enable_output(accio2_pin, false);
	
	// TODO: ingore first frame end
	
	// enable interrupts
	sei();
	//GIMSK  |= (1<<INT0);
	
	// TODO: abstract?
	
	// enable INT0 falling edge
	MCUCR = (1 << ISC01) | (1 << ISC00);
	EIMSK = (1 << INT0);
	
	while (true)
		loop();
}

void loop()
{
	// wait for frame end
	while (!frame_end)
	{}
	frame_end = false;
	
	// read byte
	digital_write(input_enable_pin, false);
	
	// TODO: don't need full ms
	_delay_ms(1);
	
	uint8_t read_byte = 0;
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], false);
		read_byte |= digital_read(data_pins[i]) << i;
	}
	digital_write(input_enable_pin, true);
	
	// write byte
	uint8_t write_byte = 0x81;
	
	// prepare output byte
	for (uint8_t i = 0; i != 8; ++i)
	{
		enable_output(data_pins[i], true);
		digital_write(data_pins[i], write_byte & (1 << i));
	}
	digital_write(output_load_pin, false);
	digital_write(output_load_pin, true);
	
	//_delay_ms(4);
	
	// accio low
	digital_write(accio_pin, false);
}

ISR (INT0_vect)
{
	// make accio high
	digital_write(accio_pin, true);
	
	frame_end = true;
}
