/********************************************
*
* Implementation of menu functions
* Commands passed to DataLayer
*
********************************************/

#include <error_defs.h>
#include <instructionSet.h>
#include <string.h>

#include "../../DataLayer/include/dataLayer_USART.h"
#include "../include/applicationLayer_USART.h"

unsigned char obd_fast_init();

void init_applicationLayer() {
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
	unsigned char data[4];
	data[0] = 0xC1;
	data[1] = 0x33;
	data[2] = 0xF1;
	data[3] = 0x81;

	usart_send_instruction(data, 4, FAST_INIT_INSTRUCTION);
	unsigned char error = usart_receive_data();

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
	unsigned char data[4];
	data[0] = 0x82;
	data[1] = 0x01;
	data[2] = 0xF1;
	data[3] = 0x01;
	data[4] = 0x00;

	usart_send_command(data, 5);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
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
	unsigned char data[4];
	data[0] = 0xC1;
	data[1] = 0x01;
	data[2] = 0xF1;
	data[3] = 0x82;

	usart_send_command(data, 4);
	unsigned char error = usart_receive_data();

	// USART Checksum error
	if (error != CODE_OK) {
		return error;
	}

	return msg.type;
}
