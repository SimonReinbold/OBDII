#include <stdlib.h>
#include <error_defs.h>

#include "../include/dataLayer_KWP2000.h"
#include "../../PhysicalLayer/include/physicalLayer_KWP2000.h"

unsigned char error;

unsigned char obd_fast_init();

char parse_format_byte(unsigned char format_byte);
void clear_msg(void);

/*********************************************************************
** Init Layers
*********************************************************************/
void init_dataLayer(){
	init_physicalLayer();
	clear_msg();
}

/*********************************************************************
** Transmit Wake-Up Patter
*
* returns: 
*********************************************************************/
void wake_up_unit() {
	wake_up();
}

/*********************************************************************
** Initialise Communication using "Fast Init"
*
* returns: CODE_OK or specific error code
*********************************************************************/
unsigned char start_communication_fastInit(unsigned char* data, unsigned char nbytes) {
	
	wake_up();

	error = send_msg(data, nbytes);
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg();
	if (error != CODE_OK) {
		return error;
	}

	// Check for negative response
	if (incoming.service_id != 0xC1) {
		return CODE_NEGATIVE_RESPONSE;
	}

	return error;
}

void clear_msg(void) {
	incoming.format_byte = 0;
	incoming.target = 0;
	incoming.source = 0;
	incoming.header_size = 0;
	incoming.length = 0;
	incoming.service_id = 0;
	incoming.checksum = 0;
	incoming.dataStreamLength = 0;
}

/*********************************************************************
** Receive message
*********************************************************************/
unsigned char receive_msg() {
	unsigned char checksum = 0;
	clear_msg();

	// Get Format byte
	error = receive_byte();

	if (error != CODE_OK) {
		return error;
	}
	incoming.format_byte = incoming_byte;
	incoming.dataStream[0] = incoming.format_byte;
	checksum += incoming.format_byte;

	/* Get header size
	*
	*	1: FMT SID DATA CS
	*	2: FMT LEN SID DATA CS
	*	3: FMT TGT SRC SID DATA CS
	*	4: FMT TGT SRC LEN SID DATA CS
	*
	*/

	incoming.header_size = parse_format_byte(incoming.format_byte);

	/*
	* Fill the message struct step by step
	*/

	// Fill target and source
	if (incoming.header_size >= 3) {
		error = receive_byte();

		if (error != CODE_OK) {
			return error;
		}

		incoming.target = incoming_byte;
		incoming.dataStream[1] = incoming.target;
		checksum += incoming.target;

		error = receive_byte();

		if (error != CODE_OK) {
			return error;
		}

		incoming.source = incoming_byte;
		incoming.dataStream[2] = incoming.source;
		checksum += incoming.source;
	}

	// Length byte included
	if (incoming.header_size == 2 || incoming.header_size == 4) {
		error = receive_byte();

		if (error != CODE_OK) {
			return error;
		}

		incoming.length = incoming_byte;
		incoming.dataStream[incoming.header_size - 1] = incoming.length;
		checksum += incoming.length;
		
	}
	else {
		incoming.length = incoming.format_byte & 0x3F;
	}
	incoming.dataStreamLength = incoming.header_size + incoming.length + 1;

	// SID, Data and Checksum left 
	for (int data_idx = 0; data_idx < incoming.length; data_idx++) {
		error = receive_byte();

		if (error != CODE_OK) {
			return error;
		}

		if (data_idx == 0) {
			// SID
			incoming.service_id = incoming_byte;
		}
		else {
			// Data bytes
			incoming.data[data_idx - 1] = incoming_byte;
		}
		incoming.dataStream[incoming.header_size + data_idx] = incoming_byte;
		checksum += incoming_byte;
	}

	// Checksum
	error = receive_byte();

	if (error != CODE_OK) {
		return error;
	}

	incoming.checksum = incoming_byte;
	incoming.dataStream[incoming.header_size + incoming.length] = incoming.checksum;

	checksum %= 256;

	// Compare checksum value
	if (incoming.checksum != checksum) {
		return CODE_CHECKSUM_ERROR_KWP2000;
	}

	// Check for negative response
	if (incoming.service_id == ID_NEGATIVE_RESPONSE) {
		if (incoming.length > 1 && 
			( incoming.data[1] == ID_NO_SUPPORT1 || 
				incoming.data[1] == ID_NO_SUPPORT2 || 
				incoming.data[1] == ID_NO_SUPPORT3 || 
				incoming.data[1] == ID_NO_SUPPORT4)) {
			return CODE_NOT_SUPPORTED;
		}
		return CODE_NEGATIVE_RESPONSE;
	}

	return error;
}

/*********************************************************************
** Send message
*********************************************************************/
unsigned char send_msg(unsigned char *data, unsigned char n_bytes) {
	// Calculate checksum while sending byte by byte
	unsigned char checksum;
	checksum = 0;
	for (unsigned int i = 0; i < n_bytes; i++) {
		checksum += *data;
		
		error = send_byte(*data++);

		if (error != CODE_OK) {
			return error;
		}
	}
	checksum %= 256;
	// Send last byte - checksum
	error = send_byte(checksum);

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

unsigned char* getIncomingData() {
	return incoming.data;
}

unsigned char getIncomingDataLength() {
	return (incoming.length - 1); // Data length excluding the SID
}