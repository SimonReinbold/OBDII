#include <util/delay.h>
#include <error_defs.h>

#include "../../../LCD/lcd-routines.h"
#include "../include/applicationLayer.h"
#include "../../DataLayer/include/dataLayer.h"

void decodeError(unsigned char error);


void init_obd() {
	init_dataLayer();
	received_data = incoming.data;
	received_nbytes = &incoming.length;
}

unsigned char init_diagnose(unsigned char* data, unsigned char nbytes){
	return start_communication_fastInit(data, nbytes);
}

unsigned char parseRequest(unsigned char* request, unsigned char nbytes) {
	unsigned char error;
	error = handleRequest(request, nbytes);
	return error;
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
	case CODE_BUS_ERROR_START:
		lcd_string("BUS ERROR START");
		break;
	case CODE_BUS_ERROR_STOP:
		lcd_string("BUS ERROR STOP");
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