
#pragma once

// 1 = pressed
// 0 = released

// disconnected (non-virtual) controller's buttons show as pressed
// except for the "spare" byte

static const uint8_t thumbpad_button_count = 17;

enum thumbpad_button : uint8_t
{
	select = 0, // change channel
	shoulder_left = 1, // set last channel
	mode = 2, // used channel sharing (B = pressed)
	reserved_14 = 3,
	is16sel_pad = 4,
	dpad_up = 5, // forward
	dpad_down = 6, // backward
	dpad_right = 7, // turn right
	dpad_left = 8, // turn left
	a = 9, // arm down
	b = 10, // arm up
	x = 11, // device forward 
	y = 12, // device backward
	reserved_12 = 13,
	reserved_13 = 14,
	shoulder_right = 15, // used to move slower
	spare = 16, // (zero)
};