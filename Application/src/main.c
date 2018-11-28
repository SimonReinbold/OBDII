//#include "Protocol/DataLayer/include/dataLaver.h"
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

#include "../../Protocol/ApplicationLayer/include/applicationLayer.h"
#include "../../LCD/lcd-routines.h"

void boot();
void show_version();

#define XSTR(x) #x
#define STR(x) XSTR(x)

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
	char version[6];
	strcpy(version, STR(GIT_VERSION));
	for (int i = 0; i < 6; i++) {
		lcd_data(version[i]);
	}

	_delay_ms(4000);
	lcd_clear();
}