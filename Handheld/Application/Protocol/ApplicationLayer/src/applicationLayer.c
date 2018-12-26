/********************************************
*
* Implementation of menu functions
* Commands passed to DataLayer
*
********************************************/

#include <error_defs.h>
#include <instructionSet.h>
#include <string.h>

#include "../../DataLayer/include/dataLayer.h"
#include "../include/applicationLayer.h"

unsigned char obd_fast_init();

unsigned char data_buffer[255];

void init_applicationLayer() {
	init_dataLayer();
}

/*********************************************************************
** Start Communication
*********************************************************************/
unsigned char start_communication_fastInit() {
	unsigned char error = obd_fast_init();

	// Check for positive response
	if (error == CODE_OK && data_buffer[0] != CODE_OK) {
		error = CODE_NEGATIVE_RESPONSE;
	}

	return error;
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
	return usart_receive_data(data_buffer);
	
}

/*********************************************************************
** Service 1 PID 00
*********************************************************************/
unsigned char requestPIDs() {
	unsigned char data[4];
	data[0] = 0xC2;
	data[1] = 0x33;
	data[2] = 0xF1;
	data[3] = 0x01;
	data[4] = 0x00;

	usart_send_command(data, 5);
	unsigned char error = usart_receive_data(data_buffer);

	// Check for positive response
	if (error == CODE_OK && data_buffer[0] != CODE_OK) {
		error = CODE_NEGATIVE_RESPONSE;
	}

	return error;
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
	data[1] = 0x33;
	data[2] = 0xF1;
	data[3] = 0x82;

	usart_send_command(data, 4);
	unsigned char error = usart_receive_data(data_buffer);

	// Check for positive response
	if (error == CODE_OK && data_buffer[0] != CODE_OK) {
		error = CODE_NEGATIVE_RESPONSE;
	}

	return error;
}
