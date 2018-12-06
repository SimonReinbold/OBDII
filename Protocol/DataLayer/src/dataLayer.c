#include <stdlib.h>

#include "../../error_defs.h"
#include "../include/dataLayer.h"
#include "../../PhysicalLayer/include/physicalLayer.h"

unsigned char error;

unsigned char obd_fast_init();

unsigned char receive_msg(int bitRate);
unsigned char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate);
char parse_format_byte(unsigned char format_byte);
void clear_msg(void);

struct {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[255];
	unsigned char checksum;
}incoming;

/*********************************************************************
** Init Layers
*********************************************************************/
void init_dataLayer(){
	init_physicalLayer();
}

/*********************************************************************
** Start Communication
*********************************************************************/
unsigned char start_communication_fastInit() {
	error = obd_fast_init();

	// Check for positive response
	if (error == CODE_OK && incoming.service_id != 0xC1) {
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

	error = send_msg(data, 4, FAST_INIT_BITRATE);
	
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg(FAST_INIT_BITRATE);
	
	// Check for positive response
	if (error == CODE_OK && incoming.service_id != 0x7F) {
		error = CODE_NEGATIVE_RESPONSE;
	}
	
	return error;
}

/*********************************************************************
** Initialise Communication using "Fast Init"
*********************************************************************/
unsigned char obd_fast_init() {
	
	wake_up();

	// Send Start Communication Request
	//
	//**								Functional Addressing | Target | Source | Service ID | Checksum
	//** Start Communication Pattern:           0xC1         |  0x33  |  0xF1  |   0x81     |   TBD
	unsigned char data[4];
	data[0] = 0xC1;
	data[1] = 0x33;
	data[2] = 0xF1;
	data[3] = 0x81;

	error = send_msg(data, 4, FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}
	
	// Receive Section
	error = receive_msg(FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}
}

void clear_msg(void) {
	incoming.format_byte = 0;
	incoming.target = 0;
	incoming.source = 0;
	incoming.service_id = 0;
	for (int i = 0; i < 33; i++) {
		incoming.data[i] = 0;
	}
	incoming.checksum = 0;
}

/*********************************************************************
** Receive message
*********************************************************************/
unsigned char receive_msg(int bitRate) {
	unsigned char checksum = 0;
	unsigned char length;

	clear_msg();

	// Get Format byte
	error = receive_byte(bitRate);

	if (error != CODE_OK) {
		return error;
	}
	incoming.format_byte = incoming_byte;
	checksum += incoming.format_byte;

	char case_idx;
	case_idx = parse_format_byte(incoming.format_byte);

	unsigned char buffer[4];

	// Get next four bytes
	for (int i = 0; i < 4; i++) {
		error = receive_byte(bitRate);

		if (error != CODE_OK) {
			return error;
		}

		buffer[i] = incoming_byte;
	}

	switch (case_idx)
	{
	case 1:
		incoming.service_id = buffer[0];
		length = incoming.format_byte & 0x3F;
		break;
	case 2:
		incoming.length = buffer[0];
		length = incoming.length;
		incoming.service_id = buffer[1];
		break;
	case 3:
		incoming.target = buffer[0];
		incoming.source = buffer[1];
		incoming.service_id = buffer[2];
		length = incoming.format_byte & 0x3F;
		break;
	case 4:
		incoming.target = buffer[0];
		incoming.source = buffer[1];
		incoming.length = buffer[2];
		length = incoming.length;
		incoming.service_id = buffer[3];
		break;
	}

	checksum += incoming.target;
	checksum += incoming.source;
	checksum += incoming.length;
	checksum += incoming.service_id;

	// Fill residual bytes into data array
	int data_idx;
	for (data_idx = 0; data_idx < length - 1; data_idx++) {
		
		// Get leftover buffer and fill data array
		if (data_idx < 4 - case_idx) {
			incoming.data[data_idx] = buffer[case_idx + data_idx];
		}
		// continue reading bytes
		else {
			error = receive_byte(bitRate);

			if (error != CODE_OK) {
				return error;
			}

			incoming.data[data_idx] = incoming_byte;
		}
		checksum += incoming.data[data_idx];
	}

	if (case_idx == 3) {
		incoming.checksum = buffer[3];
	}
	else {
		// Receive checksum byte
		error = receive_byte(bitRate);
		if (error != CODE_OK) {
			return error;
		}
		incoming.checksum = incoming_byte;
	}
	
	checksum %= 256;
	
	if (incoming.checksum != checksum) {
		error = CODE_CHECKSUM_ERROR;
	}

	return error;
}

/*********************************************************************
** Send message
*********************************************************************/
unsigned char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate) {
	if (n_bytes > 265) return CODE_DATA_ERROR;

	// Calculate checksum while sending byte by byte
	unsigned char checksum;
	checksum = 0;
	for (unsigned int i = 0; i < n_bytes; i++) {
		checksum += *data;
		
		error = send_byte(*data++, bitRate);

		if (error != CODE_OK) {
			return error;
		}
	}
	checksum %= 256;
	// Send last byte - checksum
	error = send_byte(checksum, bitRate);

	return error;
}

char parse_format_byte(unsigned char format_byte) {

	char address_flag = format_byte & 0xC0;
	char length_byte_flag = !(format_byte & 0x3F);

	char case_idx;
	if (!address_flag && !length_byte_flag) {
		case_idx = 1;
	}
	if (!address_flag && length_byte_flag) {
		case_idx = 2;
	}
	if (address_flag && !length_byte_flag) {
		case_idx = 3;
	}
	if (address_flag && length_byte_flag) {
		case_idx = 4;
	}
	return case_idx;
}