
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

uint16_t const timer_start = 4;
uint16_t const timer_mid = 6;
uint16_t timer = timer_start;
uint16_t const timer_wrap = 8;

template <typename T>
T clamp(T val, T min, T max)
{
	if (val < min)
		return min;
	else if (max < val)
		return max;
	else
		return val;
}

void fill_pad_data(uint8_t(&buttons)[thumbpad_button_count])
{
	for (auto b : buttons)
		b = 0;
	
	nc.read();
	
	uint8_t const modify_controller = 4;
	
	uint8_t const center = 0x7f;
	uint8_t const deadzone = 0x30;
	
	uint8_t const accctr = 0x7f;
	uint8_t const accdz = 0x20;
	
	//uint8_t const range = 0x7f;
	
	//for (uint8_t i = 0; i != thumbpad_button_count; ++i)
		//buttons[i] = 0;
	
	bool const tilt_up = nc.get_ay() < (accctr - accdz);
	bool const tilt_down = nc.get_ay() > (accctr + accdz);
	
	bool const tilt_left = nc.get_ax() < (center - accdz);
	bool const tilt_right = nc.get_ax() > (center + accdz);
	
	bool tool_up = 0;
	bool tool_down = 0;
	bool tool_primary = 0;
	bool tool_secondary = 0;
	bool move_n = 0;
	bool move_s = 0;
	bool move_w = 0;
	bool move_e = 0;
	
	//tool_up |= nc.get_bc();
	tool_up |= tilt_up;
	
	//tool_down |= nc.get_bz();
	tool_down |= tilt_down;
	
	tool_primary |= nc.get_bz();
	tool_secondary |= nc.get_bc();
	
	move_n |= nc.get_jy() > (center + deadzone);
	
	move_s |= nc.get_jy() < (center - deadzone);
	
	move_w |= nc.get_jx() < (center - deadzone);
	//move_w |= tilt_left;
	
	move_e |= nc.get_jx() > (center + deadzone);
	//move_e |= tilt_right;
	
	//auto up_val = (clamp(nc.get_jy() - center, 0, 0xff) * (uint16_t)20 / range);
	//if (up_val > timer_mid)
	
	//move_n |= up_val > timer;
	//move_s |= (clamp(center - nc.get_jy(), 0, 0xff) * (uint16_t)20 / range) > timer;
	
	set_bit(buttons[a], modify_controller, tool_down);
	set_bit(buttons[b], modify_controller, tool_up);
	set_bit(buttons[x], modify_controller, tool_secondary);
	set_bit(buttons[y], modify_controller, tool_primary);
	
	set_bit(buttons[dpad_up], modify_controller, move_n);
	set_bit(buttons[dpad_down], modify_controller, move_s);
	set_bit(buttons[dpad_left], modify_controller, move_w);
	set_bit(buttons[dpad_right], modify_controller, move_e);
	
	// allow other pads to control same vehicle
	set_bit(buttons[mode], modify_controller, true);
	
	if (timer_wrap == ++timer)
		timer = timer_start;
	//++timer;
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
