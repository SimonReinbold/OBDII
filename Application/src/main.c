//#include "Protocol/DataLayer/include/dataLaver.h"
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <string.h>

#include "../../Protocol/ApplicationLayer/include/applicationLayer.h"
#include "../../LCD/lcd-routines.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

void boot();
void show_version();

const char version[] = STR(GIT_VERSION);


int main() {
	boot();
	//DDRC |= 1 << PC5;

	init_diagnose();

	//PORTC |= _BV(PORTC5);
	//PORTC &= ~_BV(PORTC5);
}

void boot() {
	lcd_init();
	init_obd();
	show_version();
}

void show_version() {
	lcd_setcursor(0, 1);
	lcd_string("OBD2 Diagnose");

	lcd_setcursor(0, 2);
	for (int i = 0; i < VERS_CHARS; i++) {
		lcd_data(version[i]);
	}

	_delay_ms(2000);
	lcd_clear();
}