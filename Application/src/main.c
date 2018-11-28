//#include "Protocol/DataLayer/include/dataLaver.h"
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#include "../../Protocol/ApplicationLayer/include/applicationLayer.h"
#include "../../LCD/lcd-routines.h"

void boot();
void show_version();

int main() {
	boot();
	
	DDRC |= _BV(DDC5);
	
	init_obdii();
		
	PORTC |= _BV(PORTC5);
	//PORTC &= ~_BV(PORTC5);
}

void boot() {
	lcd_init();
	show_version();
}

void show_version() {
	lcd_setcursor(0, 1);
	lcd_string("OBD2 Diagnose");

	lcd_setcursor(0, 2);
	lcd_string("Simon Reinbold");

	_delay_ms(3000);
	lcd_clear();

	lcd_setcursor(0, 1);
	lcd_string("Version");
	
	unsigned char major;
	unsigned char minor;
	unsigned char patch;

	itoa(MAJOR, &major, 10);
	itoa(MINOR, &minor, 10);
	itoa(PATCH, &patch, 10);

	lcd_setcursor(0, 2);
	lcd_data(major);
	lcd_data('.');
	lcd_data(minor);
	lcd_data('.');
	lcd_data(patch);

	_delay_ms(3000);
	lcd_clear();
}