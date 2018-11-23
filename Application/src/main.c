//#include "Protocol/DataLayer/include/dataLaver.h"
#include <avr/io.h>
#include "../../Protocol/PhysicalLayer/include/physicalLayer.h"
#include "../../Protocol/DataLayer/include/dataLaver.h"
#include "../../LCD/lcd-routines.h"

int main() {
	lcd_init();
	init_dataLayer();
	
	DDRB |= _BV(DDB1);
	/* set pin 5 high to turn led on */
	unsigned char err;
	err = obd_fast_init();
	
	display(&err,1,1);

	PORTB |= _BV(PORTB1);
	/* set pin 5 low to turn led off */
	//PORTB &= ~_BV(PORTB1);
}