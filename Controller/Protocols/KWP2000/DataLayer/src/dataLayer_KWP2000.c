#include <stdlib.h>
#include <error_defs.h>

#include "../include/dataLayer_KWP2000.h"
#include "../../PhysicalLayer/include/physicalLayer_KWP2000.h"

unsigned char error;

unsigned char obd_fast_init();

unsigned char receive_msg(int bitRate);
unsigned char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate);
char parse_format_byte(unsigned char format_byte);
void clear_msg(void);

/*********************************************************************
** Init Layers
*********************************************************************/
void init_dataLayer(){
	init_physicalLayer();
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

	error = send_msg(data, nbytes, FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg(FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}

	// Check for negative response
	if (incoming.service_id != 0xC1) {
		return CODE_NEGATIVE_RESPONSE;
	}

	return error;
}

/*********************************************************************
** Handle Request
*********************************************************************/
unsigned char handleRequest(unsigned char* request, unsigned char nbytes) {
	error = send_msg(request, nbytes, FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg(FAST_INIT_BITRATE);
	if (error != CODE_OK) {
		return error;
	}

	return error;
}

void clear_msg(void) {
	incoming.format_byte = 0;
	incoming.target = 0;
	incoming.source = 0;
	incoming.length = 0;
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
	incoming.dataStreamLength = 0;

	clear_msg();
	unsigned char* dataStreamPtr = &incoming.dataStream[0];

	// Get Format byte
	error = receive_byte(bitRate);

	if (error != CODE_OK) {
		return error;
	}
	incoming.format_byte = incoming_byte;
	*dataStreamPtr++ = incoming_byte;
	(incoming.dataStreamLength)++;
	checksum += incoming.format_byte;

	/* Get case idx
	*
	*	1: FMT SID DATA CS
	*	2: FMT LEN SID DATA CS
	*	3: FMT TGT SRC SID DATA CS
	*	4: FMT TGT SRC LEN SID DATA CS
	*
	*/
	char case_idx = parse_format_byte(incoming.format_byte);

	/*
	* Fill the message struct step by step
	*/

	// Fill target and source
	if (case_idx == 3 || case_idx == 4) {
		error = receive_byte(bitRate);

		if (error != CODE_OK) {
			return error;
		}

		incoming.target = incoming_byte;
		*dataStreamPtr++ = incoming_byte;
		(incoming.dataStreamLength)++;
		checksum += incoming.target;

		error = receive_byte(bitRate);

		if (error != CODE_OK) {
			return error;
		}

		incoming.source = incoming_byte;
		*dataStreamPtr++ = incoming_byte;
		(incoming.dataStreamLength)++;
		checksum += incoming.source;
	}

	// Length byte included
	if (case_idx == 2 || case_idx == 4) {
		error = receive_byte(bitRate);

		if (error != CODE_OK) {
			return error;
		}

		incoming.length = incoming_byte;
		*dataStreamPtr++ = incoming_byte;
		(incoming.dataStreamLength)++;
		checksum += incoming.length;
		length = incoming.length;
	}
	else {
		length = incoming.format_byte & 0x3F;
	}

	// SID and data left 
	// defined by length variable filled with either the length byte or the length defined in FMT

	for (int data_idx = 0; data_idx < length; data_idx++) {
		error = receive_byte(bitRate);

		if (error != CODE_OK) {
			return error;
		}

		if (data_idx == 0) {
			// SID
			incoming.service_id = incoming_byte;
			*dataStreamPtr++ = incoming_byte;
			(incoming.dataStreamLength)++;
			checksum += incoming.service_id;
		}
		else {
			// Data bytes
			incoming.data[data_idx - 1] = incoming_byte;
			*dataStreamPtr++ = incoming_byte;
			(incoming.dataStreamLength)++;
			checksum += incoming.data[data_idx - 1];
		}
		
	}

	// Checksum
	error = receive_byte(bitRate);

	if (error != CODE_OK) {
		return error;
	}

	incoming.checksum = incoming_byte;
	*dataStreamPtr++ = incoming_byte;
	(incoming.dataStreamLength)++;

	// Compare checksum value
	if (incoming.checksum != checksum) {
		error = CODE_CHECKSUM_ERROR_KWP2000;
	}

	return error;
}

/*********************************************************************
** Send message
*********************************************************************/
unsigned char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate) {
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