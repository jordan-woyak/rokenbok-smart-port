
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <util/setbaud.h>

//#include <stdio.h>
//#include <stdlib.h>

#include "usart.hpp"
#include "rokenbok_interface.hpp"
#include "smart_port.hpp"

void loop();

smart_port sp;

int main()
{
	//free(malloc(5));
	
	//USART_init();
	
	sp.init();
	
	rokenbok_interface rokinf;
	rokinf.process(sp);
}
