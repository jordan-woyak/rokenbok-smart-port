
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <util/setbaud.h>

#include <stdio.h>
//#include <stdlib.h>

#include "bit_util.hpp"

#include "thumbpad.hpp"
#include "nunchuk.hpp"

nunchuk nc;

void fill_pad_data(uint8_t(&buttons)[thumbpad_button_count])
{
	for (auto b : buttons)
		b = 0;
	
	nc.read();
	
	uint8_t const modify_controller = 4;
	uint8_t const center = 0xff/2;
	uint8_t const deadzone = 0x30;
	uint8_t const accdz = 0x20;
	
	//for (uint8_t i = 0; i != thumbpad_button_count; ++i)
		//buttons[i] = 0;
	
	bool const tilt_up = nc.get_ay() < (center - accdz);
	bool const tilt_down = nc.get_ay() > (center + accdz);
	
	bool const tilt_left = nc.get_ax() < (center - accdz);
	bool const tilt_right = nc.get_ax() > (center + accdz);
	
	set_bit(buttons[a], modify_controller, nc.get_bz() | tilt_down);
	set_bit(buttons[b], modify_controller, nc.get_bc() | tilt_up);
	
	set_bit(buttons[dpad_down], modify_controller, nc.get_jy() < (center - deadzone));
	set_bit(buttons[dpad_up], modify_controller, nc.get_jy() > (center + deadzone));
	set_bit(buttons[dpad_left], modify_controller, nc.get_jx() < (center - deadzone) | tilt_left);
	set_bit(buttons[dpad_right], modify_controller, nc.get_jx() > (center + deadzone) | tilt_right);
}

#include "usart.hpp"
#include "rokenbok_interface.hpp"
#include "smart_port.hpp"

void loop();

smart_port sp;

int main()
{	
	USART_init();
	
	sp.init();
	
	rokenbok_interface rokinf;
	rokinf.process(sp);
}
