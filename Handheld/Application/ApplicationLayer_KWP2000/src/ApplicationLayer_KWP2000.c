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
#include <stdlib.h>
#include <ctype.h>

#include "../include/ApplicationLayer_KWP2000.h"
#include "../../../Application/Buttons/buttonHandler.h"

// Display periodic requests
#include <util/delay.h>
#include "../../../LCD/lcd-routines.h"

#define STANDARD_UPDATE_RATE_ms					500

#define TARGET_FAST_INIT						0x33
#define TARGET_ECU								0x01

#define SID_FAST_INIT							0x81
#define SID_SERVICE_01							0x01
#define SID_SERVICE_03							0x03
#define SID_SERVICE_0A							0x0A

#define PID_CALC_ENGINE_LOAD					0x04
#define PID_ENIGNE_COOLANT_TEMP					0x05
#define PID_INTAKE_MANIFOLD_ABOLUTE_PRESSURE	0x0B
#define PID_ENGINE_RPM							0x0C
#define PID_VEHICLE_SPEED						0x0D

#define PID_INTAKE_AIR_TEMP						0x0F
#define PID_MAF_AIR_FLOW_RATE					0x10
#define PID_THROTTLE_POSITION					0x11

#define HEADER_OFFSET							4

unsigned char obd_fast_init();

void init_applicationLayer_KWP2000() {
	// Nothing to do
}

/*********************************************************************
** Start Communication
*********************************************************************/
unsigned char start_communication_fastInit() {
	unsigned char error = obd_fast_init();
	if (error != CODE_OK) {
		return error;
	}
	return requestPIDs();
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

	for (unsigned char i = 0; i < 6; i++) {
		data[2] = i * 0x20;

		usart_send_data(REQUEST, data, 3);
		unsigned char error = usart_receive_data();

		if (error != CODE_OK && msg_USART.type != CODE_OK) {
			return error;
		}
	}

	return msg_USART.type;
}

/*********************************************************************
** Calculated Engine Load
*********************************************************************/
unsigned char calcEngineLoad() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_CALC_ENGINE_LOAD;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Calc Engine Load");

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
		else {
			// Calc Load
			unsigned char load = (unsigned char)( ((unsigned int)msg_USART.data[HEADER_OFFSET + 1]*(unsigned int)100)/255 ); // In %

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Calc Engine Load");
			lcd_setcursor(0, 2);
			lcd_append_decimal(load);
			lcd_data(' ');
			lcd_data(0x25); // % sign
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Engine Coolant Temperature
*********************************************************************/
unsigned char engineCoolantTemp() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_ENIGNE_COOLANT_TEMP;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Eng Coolant Temp");

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
		else {
			// Calc Temperature
			int temperature = (((int)msg_USART.data[HEADER_OFFSET + 1]) - 40); // A - 40 degrees celcius

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Eng Coolant Temp");
			lcd_setcursor(0, 2);
			lcd_append_decimal(temperature);
			lcd_data(' ');
			lcd_data(0b11011111);
			lcd_data('C');
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Intake Manifold Absolute Pressure
*********************************************************************/
unsigned char intakeManifoldAbsolutePressure() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_INTAKE_MANIFOLD_ABOLUTE_PRESSURE;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Intake Abs Press");

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
		else {
			// Calc Temperature
			unsigned char pressure = msg_USART.data[HEADER_OFFSET + 1]; // kPa
			double pressure_bar = (double)pressure / 100;				// bar

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Intake Abs Press");
			lcd_setcursor(0, 2);
			char buffer[5];
			dtostrf(pressure_bar, 4, 2, buffer);
			lcd_string(buffer);
			lcd_data(' ');
			lcd_string("bar");
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Engine RPM
*********************************************************************/
unsigned char engineRPM() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_ENGINE_RPM;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Engine RPM");

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
		else {
			double rpm = ((256 * (double)msg_USART.data[HEADER_OFFSET + 1]) + (double)msg_USART.data[HEADER_OFFSET + 2]) / 4;

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Engine RPM");
			lcd_setcursor(0, 2);
			char buffer[5];
			dtostrf(rpm, 4, 0, buffer);
			lcd_string(buffer);
			lcd_data(' ');
			lcd_string("rpm");
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Vehicle Speed
*********************************************************************/
unsigned char vehicleSpeed() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_VEHICLE_SPEED;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Vehicle Speed");

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
		else {
			unsigned char speed = msg_USART.data[HEADER_OFFSET + 1]; // km/h

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Vehicle Speed");
			lcd_setcursor(0, 2);
			lcd_append_decimal(speed);
			lcd_string(" km/h");
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Show Intake Air Temp
*********************************************************************/
unsigned char intake_air_Temp() {
	unsigned char data[3];
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
			int temperature = (((int)msg_USART.data[HEADER_OFFSET+1])-40); // A - 40 degrees celcius

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
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Mass Air Flow Sensor air flow rate
*********************************************************************/
unsigned char mafAirFlowRate() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_MAF_AIR_FLOW_RATE;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Air Flow Rate");

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
		else {
			double rate = ((256 * (double)msg_USART.data[HEADER_OFFSET + 1]) + (double)msg_USART.data[HEADER_OFFSET + 2]) / 100;

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Air Flow Rate");
			lcd_setcursor(0, 2);
			char buffer[4];
			dtostrf(rate, 3, 0, buffer);
			lcd_string(buffer);
			lcd_string(" g/s");
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
	}

	return msg_USART.type;
}

/*********************************************************************
** Throttle Position
*********************************************************************/
unsigned char throttlePosition() {
	unsigned char data[3];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_01;
	data[2] = PID_THROTTLE_POSITION;

	enableManualStop();

	// Display
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("Throttle Pos.");

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
		else {
			// Calc Load
			unsigned char load = (unsigned char)(((unsigned int)msg_USART.data[HEADER_OFFSET + 1] * (unsigned int)100) / 255); // In %

			// Display
			lcd_clear();
			lcd_setcursor(0, 1);
			lcd_string("Throttle Pos.");
			lcd_setcursor(0, 2);
			lcd_append_decimal(load);
			lcd_data(' ');
			lcd_data(0x25); // % sign
		}
		_delay_ms(STANDARD_UPDATE_RATE_ms);
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

/*********************************************************************
** DTCs
*********************************************************************/
unsigned char requestDTCs() {
	unsigned char data[2];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_03;
	
	usart_send_data(REQUEST, data, 2);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) return error;
	if (msg_USART.type != CODE_OK) return msg_USART.type;
	
	int cross_sum = 0;
	for (int i = HEADER_OFFSET; i < msg_USART.length - 1; i++) {
		cross_sum += msg_USART.data[i]; // Check if all bytes are zero -> only P0000 DTCs (not valid/no DTC)
	}

	if (msg_USART.length - 5 == 0 || cross_sum == 0) {
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("DTCs:");
		lcd_setcursor(0, 2);
		lcd_string("NO DTCs");
		_delay_ms(500);
		return CODE_OK;
	}

	// DTCs received, show in the same style as the menu
	int currentDTC = 0;
	while (1) {
		// Show current DTC
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("DTCs:");
		lcd_setcursor(0, 2);
		lcd_append_decimal((currentDTC / 2) + 1);
		lcd_data('.');
		lcd_data(' ');
		switch (msg_USART.data[currentDTC] >> 6)
		{
		case 0:
			lcd_data('P');
			break;
		case 1:
			lcd_data('C');
			break;
		case 2:
			lcd_data('B');
			break;
		case 3:
			lcd_data('U');
			break;
		default:
			break;
		}
		lcd_append_decimal((msg_USART.data[currentDTC] & 0x30) >> 4);
		char buffer[2];
		itoa(msg_USART.data[currentDTC] & 0x0F, buffer, 16);
		lcd_data(toupper(buffer[0]));
		HextoASCII(&msg_USART.data[currentDTC+1]);

		waitForButtonRelease();
		waitForButtonPress();
		waitForButtonRelease();
		switch (button_pressed) {
		case 1:
			if (currentDTC != 0) {
				currentDTC -= 2;
			}
			break;
		case 2:
			if (currentDTC < msg_USART.length - 2) {
				currentDTC += 2;
			}
			break;
		case 3:
			return CODE_OK; // Return to menu
			break;
		}
	}
}

/*********************************************************************
** Permanenet DTCs
*********************************************************************/
unsigned char requestPermDTCs() {
	unsigned char data[2];
	data[0] = TARGET_ECU;
	data[1] = SID_SERVICE_0A;

	usart_send_data(REQUEST, data, 2);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) return error;
	if (msg_USART.type != CODE_OK) return msg_USART.type;

	int cross_sum = 0;
	for (int i = HEADER_OFFSET; i < msg_USART.length - 1; i++) {
		cross_sum += msg_USART.data[i]; // Check if all bytes are zero -> only P0000 DTCs (not valid/no DTC)
	}

	if (msg_USART.length - 5 == 0 || cross_sum == 0) {
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("DTCs:");
		lcd_setcursor(0, 2);
		lcd_string("NO DTCs");
		_delay_ms(500);
		return CODE_OK;
	}

	// DTCs received, show in the same style as the menu
	int currentDTC = 0;
	while (1) {
		// Show current DTC
		lcd_clear();
		lcd_setcursor(0, 1);
		lcd_string("DTCs:");
		lcd_setcursor(0, 2);
		lcd_append_decimal((currentDTC / 2) + 1);
		lcd_data('.');
		lcd_data(' ');
		switch (msg_USART.data[currentDTC] >> 6)
		{
		case 0:
			lcd_data('P');
			break;
		case 1:
			lcd_data('C');
			break;
		case 2:
			lcd_data('B');
			break;
		case 3:
			lcd_data('U');
			break;
		default:
			break;
		}
		lcd_append_decimal((msg_USART.data[currentDTC] & 0x30) >> 4);
		char buffer[2];
		itoa(msg_USART.data[currentDTC] & 0x0F, buffer, 16);
		lcd_data(toupper(buffer[0]));
		HextoASCII(&msg_USART.data[currentDTC + 1]);

		waitForButtonRelease();
		waitForButtonPress();
		waitForButtonRelease();
		switch (button_pressed) {
		case 1:
			if (currentDTC != 0) {
				currentDTC -= 2;
			}
			break;
		case 2:
			if (currentDTC < msg_USART.length - 2) {
				currentDTC += 2;
			}
			break;
		case 3:
			return CODE_OK; // Return to menu
			break;
		}
	}
}