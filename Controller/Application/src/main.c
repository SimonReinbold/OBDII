/**************************************************************
*
*
*                        CONTROLLER
*
*
***************************************************************/
#include <util/delay.h>
#include <avr/io.h>

#include "../../USART/DataLayer/include/dataLayer_USART.h"
#include "../../KWP2000/ApplicationLayer/include/applicationLayer_KWP2000.h"

void boot();

int main() {
	boot();


	while (1) {
		// Execute request
		executeRequest();
		
		// Transmit reply
		reply();
	}
}

void boot() {
	init_obd();
	init_dataLayer_USART();

	// Debug LED 
	DDRD |= 1 << PD7;
	PORTD &= ~(1 << PD7);
}