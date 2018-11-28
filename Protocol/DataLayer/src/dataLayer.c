#include "../include/dataLaver.h"
#include "../../PhysicalLayer/include/physicalLayer.h"
#include "../../../Application/include/error_defs.h"

unsigned char error;

struct {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[255]; // Filled by INT0 interrupt
	unsigned char checksum;
}incoming;

unsigned char obd_fast_init();
unsigned char receive_msg(int bitRate);
char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate);
unsigned char receive_msg(int bitRate);

/*********************************************************************
** Init Layers
*********************************************************************/
void init_dataLayer(){
	obd_hardware_init();
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
	error = send_msg(data, 4, 10400);
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg(10400);

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

	error = send_msg(data, 4, 10400);
	if (error != CODE_OK) {
		return error;
	}

	// Receive Section
	error = receive_msg(10400);
	if (error != CODE_OK) {
		return error;
	}
}

/*********************************************************************
** Receive message
*********************************************************************/
unsigned char receive_msg(int bitRate) {

	unsigned char length_byte_flg = 0;
	unsigned char address_flg = 0;
	unsigned char receive_in_progress = 1;
	unsigned char byte_cnt = 0;
	unsigned char checksum = 0;

	// Bytes of message excluding the checksum
	unsigned char message_length = 0;

	// Get Format byte
	if (receive_byte(bitRate) == CODE_OK) {
		byte_cnt++;
		incoming.format_byte = incoming_byte;
		checksum += incoming_byte;
		message_length++;
	}
	// Parse Formate Byte
	if ((incoming.format_byte & 0xC0)) {
		// Target and source byte included
		address_flg = 1;
		message_length += 2;
	}
	
	incoming.length = incoming.format_byte & 0x3F;
	message_length += incoming.length;
	
	if (incoming.length == 0) {
		// Length byte included
		length_byte_flg = 1;
		message_length += 1;
	}

	// Add service_ID byte to counter
	message_length += 1;
	
	unsigned char* incoming_data_ptr = &incoming.data[0];
	
	while (receive_in_progress) {
		// Receive byte
		error = receive_byte(bitRate);
		if (error != CODE_OK) {
			break;
		}
		byte_cnt++;

		switch (byte_cnt)
		{
		case 2:
			if (address_flg) {
				incoming.target = incoming_byte;
			}
			else if(length_byte_flg){
				incoming.length = incoming_byte;
				message_length += incoming.length;
			}
			else {
				incoming.service_id = incoming_byte;
			}
			checksum += incoming_byte;
			break;
		case 3:
			if (address_flg) {
				incoming.source = incoming_byte;
			}
			else if (length_byte_flg) {
				incoming.service_id = incoming_byte;
			}
			else {
				*incoming_data_ptr = incoming_byte;
				incoming_data_ptr++;
			}
			checksum += incoming_byte;
			break;
		case 4:
			if (address_flg && length_byte_flg) {
				incoming.length = incoming_byte;
			}
			else if (address_flg && !length_byte_flg) {
				incoming.service_id = incoming_byte;
			}
			else {
				*incoming_data_ptr = incoming_byte;
				incoming_data_ptr++;
			}
			checksum += incoming_byte;
			break;
		case 5:
			if (address_flg && length_byte_flg) {
				incoming.service_id = incoming_byte;
			}
			else {
				*incoming_data_ptr = incoming_byte;
				incoming_data_ptr++;
			}
			checksum += incoming_byte;
			break;
		default:
			if (byte_cnt > message_length) {
				incoming.checksum = incoming_byte;
				receive_in_progress = 0;
			}
			else {
				*incoming_data_ptr = incoming_byte;
				checksum += incoming_byte;
				incoming_data_ptr++;
			}
			break;
		}
	}

	// Check checksum
	checksum %= 256;
	if (!checksum == incoming.checksum) {
		error = CODE_CHECKSUM_ERROR;
	}
	
	return error;
}

/*********************************************************************
** Send message
*********************************************************************/
char send_msg(unsigned char *data, unsigned char n_bytes, int bitRate) {
	if (n_bytes > 265) return CODE_DATA_ERROR;

	// Calculate checksum while sending byte by byte
	unsigned char checksum;
	for (unsigned int i = 0; i < n_bytes; i++) {
		checksum += *data;
		error = send_byte(*data++, bitRate);
		if (error != CODE_OK) {
			return error;
		}
	}
	// Send last byte - checksum
	error = send_byte(checksum, bitRate);

	return error;
}