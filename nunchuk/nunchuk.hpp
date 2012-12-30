
#pragma once

//#include "two_wire.hpp"
#include "twimaster.hpp"

// should really only create one
class nunchuk
{
private:
	uint8_t device_address = 0xa4;
	
public:
	nunchuk()
	{
		i2c_init();
		
		uint8_t ret = i2c_start(device_address + I2C_WRITE);
		i2c_write(0x40);
		i2c_write(0x00);
		i2c_stop();	
	}

	void read()
	{
		uint8_t ret=i2c_start(device_address + I2C_WRITE);
		i2c_write(0x00);
		i2c_stop();
		_delay_us(500);
		
		i2c_start_wait(device_address + I2C_READ);
		for(uint8_t i = 0; i != 5; ++i)
			data[i] = i2c_readAck();
		data[5]= i2c_readNak();
		
		i2c_stop();
		
		// Decode data
		for (auto& b : data)
			b = (b ^ 0x17) + 0x17;	
	}
	
	uint8_t get_jx() const
	{
		return data[0];
	}
	
	uint8_t get_jy() const
	{
		return data[1];
	}
	
	uint8_t get_ax() const
	{
		return data[2];
	}
	
	uint8_t get_ay() const
	{
		return data[3];
	}
	
	uint8_t get_az() const
	{
		return data[4];
	}
	
	bool get_bc() const
	{
		return !(data[5] & (1 << 1));
	}
	
	bool get_bz() const
	{
		return !(data[5] & (1 << 0));
	}
	
private:
	uint8_t data[6];
};