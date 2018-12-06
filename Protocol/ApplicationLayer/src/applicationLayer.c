#include <util/delay.h>

#include "../../../LCD/lcd-routines.h"
#include "../../error_defs.h"
#include "../include/applicationLayer.h"
#include "../../DataLayer/include/dataLayer.h"

void decodeError(unsigned char error);

unsigned char error;

void init_obd() {
	init_dataLayer();
}

void init_diagnose(){
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("START COM");
	error = start_communication_fastInit();
	decodeError(error);

	_delay_ms(3000);
	if (error == CODE_OK) {
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("STOP COM");
		error = stop_communication();
		decodeError(error);
	}
}

void decodeError(unsigned char error) {
	lcd_setcursor(0, 2);
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
		lcd_display(&error, 1, 2);
		break;
	}
}