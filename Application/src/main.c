//#include "Protocol/DataLayer/include/dataLaver.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/delay.h>

#include "../../Protocol/DataLayer/include/dataLaver.h"
#include "../../LCD/lcd-routines.h"
#include "../include/error_defs.h"

unsigned char error;

void boot();
void show_version();

int main() {
	boot();
	init_dataLayer();
	
	DDRC |= _BV(DDC5);
	/* set pin 5 high to turn led on */	
	
	lcd_setcursor(0, 1);
	error = start_communication_fastInit(); 
	switch (error)
	{
		case CODE_OK:
			lcd_string("OK");
			break;
		case CODE_DATA_ERROR:
			lcd_string("DATA ERROR");
			break;
		case CODE_ERROR:
			lcd_string("ERROR");
			break;
		case CODE_BUS_ERROR:
			lcd_string("BUS ERROR");
			break;
		case CODE_NO_DATA:
			lcd_string("NO DATA");
			break;
		case CODE_CHECKSUM_ERROR:
			lcd_string("CHECKSUM ERROR");
			break;
		case CODE_NEGATIVE_RESPONSE:
			lcd_string("START COM FAILED");
			break;
		default:
			display(&error, 1, 1);
			break;
	}
	
	lcd_setcursor(0, 2);
	error = stop_communication();
	switch (error)
	{
	case CODE_OK:
		lcd_string("OK");
		break;
	case CODE_DATA_ERROR:
		lcd_string("DATA ERROR");
		break;
	case CODE_ERROR:
		lcd_string("ERROR");
		break;
	case CODE_BUS_ERROR:
		lcd_string("BUS ERROR");
		break;
	case CODE_NO_DATA:
		lcd_string("NO DATA");
		break;
	case CODE_CHECKSUM_ERROR:
		lcd_string("CHECKSUM ERROR");
		break;
	case CODE_NEGATIVE_RESPONSE:
		lcd_string("START COM FAILED");
		break;
	default:
		display(&error, 1, 2);
		break;
	}
	
	PORTC |= _BV(PORTC5);
	/* set pin 5 low to turn led off */
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