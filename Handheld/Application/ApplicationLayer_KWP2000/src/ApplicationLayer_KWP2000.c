/********************************************
*
* Implementation of menu functions
* Commands passed to DataLayer
*
********************************************/

#include <error_defs.h>
#include <instructionSet.h>
#include <string.h>
#include <USART/DataLayer/include/dataLayer_USART.h>

#include "../include/ApplicationLayer_KWP2000.h"
#include "../../../Application/Buttons/buttonHandler.h"

// Display periodic requests
#include <util/delay.h>
#include "../../../LCD/lcd-routines.h"

#define TARGET_FAST_INIT		0x33
#define TARGET_ECU				0x01

#define SID_FAST_INIT			0x81
#define SID_SERVICE_01			0x01

#define PID_INTAKE_AIR_TEMP		0x0F

unsigned char obd_fast_init();

void init_applicationLayer_KWP2000() {
	// Nothing to do
}

/*********************************************************************
** Start Communication
*********************************************************************/
unsigned char start_communication_fastInit() {
	return obd_fast_init();
}

/*********************************************************************
** Initialise Communication using "Fast Init"
*********************************************************************/
unsigned char obd_fast_init() {

	// Send Start Communication Request
	//
	//**								Functional Addressing | Target | Source | Service ID | Checksum
	//** Start Communication Pattern:           0xC1         |  0x33  |  0xF1  |   0x81     |   TBD
	unsigned char data[2];
	data[0] = TARGET_FAST_INIT;
	data[1] = SID_FAST_INIT;

	usart_send_data(REQUEST, data, 2);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
	}

	// No USART error, return the error code in msg.type
	return msg_USART.type;
}

/*********************************************************************
** Service 1 PID 00
*********************************************************************/
unsigned char requestPIDs() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;

	//for (unsigned char i = 0; i < 6; i++) {
		data[2] = 0x00;
		
		usart_send_data(REQUEST, data, 3);
		unsigned char error = usart_receive_data();
		
		// USART Checksum error
		if (error != CODE_OK && msg_USART.type != CODE_OK) {
			return error;
		}
	//}

	return msg_USART.type;
}

/*********************************************************************
** Show Intake Air Temp
*********************************************************************/
unsigned char intake_air_Temp() {
	unsigned char data[2];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_INTAKE_AIR_TEMP;
	
	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Intake Air Temp");

	while (1) {
		usart_send_data(REQUEST, data, 3);
		unsigned char error = usart_receive_data();

		// USART Checksum error
		if (error != CODE_OK) return error;

		if (msg_USART.type != CODE_OK) {
			lcd_setcursor(10, 2);
			lcd_data('E');
			lcd_data('r');
			lcd_data('r');
			lcd_data(' ');
			HextoASCII(&msg_USART.type);
		}
		else{
			// Calc Temperature
			int temperature = (((int)msg_USART.data[1])-40); // A - 40 degrees celcius

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Intake Air Temp");
			lcd_setcursor(0, 2);
			lcd_append_decimal(temperature);
			lcd_data(' ');
			lcd_data(0b11011111);
			lcd_data('C');
		}
		_delay_ms(500);
	}

	return msg_USART.type;
}

/*********************************************************************
** Stop Communication
*********************************************************************/
unsigned char stop_communication() {
	// Send Start Communication Request
	//
	//**								Functional Addressing | Target | Source | Service ID | Checksum
	//** Start Communication Pattern:           0xC1         |  0x33  |  0xF1  |   0x82     |   TBD
	unsigned char data[2];
	data[0] = 0x33;
	data[1] = 0x82;

	usart_send_data(REQUEST, data, 2);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
	}

	return msg_USART.type;
}
