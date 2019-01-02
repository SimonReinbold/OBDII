/********************************************
*
* Implementation of menu functions
* Commands passed to DataLayer
*
********************************************/

#include <error_defs.h>
#include <instructionSet.h>
#include <string.h>

#include "../../../USART/DataLayer/include/dataLayer_USART.h"
#include "../include/ApplicationLayer_KWP2000.h"

#include "../../Buttons/buttonHandler.h"

#include <util/delay.h>
#include "../../../LCD/lcd-routines.h"

unsigned char obd_fast_init();

void init_applicationLayer_KWP2000() {
	init_dataLayer();
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
	data[0] = 0x33;
	data[1] = 0x81;

	//enableManualStop();

	usart_send_data(data, 2, REQUEST);
	unsigned char error = usart_receive_data();
	/*
	if (isManualTriggerActive()) {
		disableManualStop();
		return CODE_MANUAL_STOP; // Return if manually stopped
	}
	*/

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
	}

	// No USART error, return the error code in msg.type
	return msg.type;
}

/*********************************************************************
** Service 1 PID 00
*********************************************************************/
unsigned char requestPIDs() {
	unsigned char data[3];
	data[0] = 0x01;
	data[1] = 0x01;

	for (unsigned char i = 0; i < 6; i++) {
		data[2] = i * 0x20;

		usart_send_data(data, 3,REQUEST);
		unsigned char error = usart_receive_data();

		// USART Checksum error
		if (error != CODE_OK) {
			return error;
		}
	}

	return msg.type;
}

/*********************************************************************
** Show Intake Air Temp
*********************************************************************/
unsigned char intake_air_Temp() {
	unsigned char data[2];
	data[0] = 0x01;
	data[1] = 0x01;
	data[2] = 0x0F;
	
	//enableManualStop();
	while (1) {
		usart_send_data(data, 3,REQUEST);
		unsigned char error = usart_receive_data();

		/*
		if (isManualTriggerActive()) {
			disableManualStop();
			return CODE_MANUAL_STOP; // Return if manually stopped
		}
		*/

		// USART Checksum error
		if (error != CODE_OK) return error;

		// Calc Temperature
		int temperature = (((int)msg.data[1])-40); // A - 40 degrees celcius

		// Display
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("Intake Air Temp");
		lcd_setcursor(0, 2);
		lcd_append_decimal(temperature);
		lcd_string(" C");
		_delay_ms(500);
	}

	return msg.type;
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

	usart_send_data(data, 2, REQUEST);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
	}

	return msg.type;
}
