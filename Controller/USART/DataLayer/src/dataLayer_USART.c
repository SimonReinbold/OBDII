#include <USART/include/USART.h>
#include <error_defs.h>
#include <instructionSet.h>

#include "../include/dataLayer_USART.h"
#include "../../../KWP2000/ApplicationLayer/include/applicationLayer_KWP2000.h"
#include "../../../KWP2000/DataLayer/include/dataLayer_KWP2000.h"

void clearMessage();
unsigned char decodeMessage();
void instruction();
void regular();
void usart_send_data(unsigned char* data, unsigned char nbytes, unsigned char type);
void usart_receive_data(unsigned char* data_buffer);

void init_dataLayer_USART() {
	USART_Init_Receiver();
	reply_data[0] = 0;
	reply_size = 0;
	reply_type = CODE_OK;
}

void clearMessage() {
	msg.checksum = 0;
	msg.type = 0;
	msg.length = 0;

	for (int i = 0; i < BUFFER_SIZE; i++) {
		msg.data[i] = 0;
	}
}

void executeRequest() {
	// Wait for instructions
	usart_receive_data(uart_buffer);

	reply_type = decodeMessage();

	// Set default error for reply
	reply_size = 0;
	
	if (reply_type != CODE_OK) {
		return;
	}
	
	// Check for instruction
	if (msg.type != REGULAR_INSTRUCTION) {
		instruction();
	}
	else {
		regular();
	}
}

void reply() {
	usart_send_data(reply_data, reply_size, reply_type);
}

/* 
* Decodes receved USART message and assigns correspondant msg struct values
*
* return: CODE_CHECKSUM_ERROR or CODE_OK
*/
unsigned char decodeMessage() {
	clearMessage();

	msg.length = uart_buffer[0];
	msg.checksum += msg.length;
	msg.type = uart_buffer[1];
	msg.checksum += msg.type;

	for (int i = 0; i < msg.length; i++) {
		msg.data[i] = uart_buffer[i+2];
		msg.checksum += msg.data[i];
	}

	if (msg.checksum != uart_buffer[msg.length + 2]) {
		return CODE_CHECKSUM_ERROR_USART;
	}
	
	return CODE_OK;
}

/*
* Executes the desired instruction including the corresponing special execution
* Calls KWP2000 Application Layer functions
*
* If return == CODE_OK set reply to CODE_SUCCESS
* Else set reply to CODE_FAILED
*
*/
void instruction() {	
	reply_size = 0; // Size is zero if request fails
	switch (msg.type) {
		case FAST_INIT_INSTRUCTION:
			reply_type = init_diagnose(msg.data, msg.length);
			//if (reply_type == CODE_OK) {
			for (unsigned char i = 0; i < incoming.dataStreamLength; i++) {
				reply_data[i] = incoming.dataStream[i];
			}
			reply_size = incoming.dataStreamLength;
			//}
	}
}

void regular() {
	reply_type = parseRequest(msg.data, msg.length);
	if (reply_type != CODE_OK) {
		reply_size = 0; // Failed request, return and transmit error code
		return;
	}
	for (unsigned char i = 0; i < incoming.dataStreamLength; i++) {
		reply_data[i] = incoming.dataStream[i];
	}
	reply_size = incoming.dataStreamLength;
}

void usart_send_data(unsigned char* data, unsigned char nbytes, unsigned char type) {
	set_Transmitter();

	char checksum = 0;

	checksum += nbytes;
	USART_Transmit(nbytes);
	checksum += type;
	USART_Transmit(type);

	for (char i = 0; i < nbytes; i++) {
		checksum += *data;
		USART_Transmit(*data++);

		if (i == nbytes - 1) {
			clearTransmitCompleteFlag();
		}
	}
	while (!(checkTransmitComplete()));
	enable_transmit_complete_Interrupt();
	USART_Transmit(checksum);
}

void usart_receive_data(unsigned char* data_buffer) {
	set_Receiver();

	char checksum = 0;
	char length;
	length = USART_Receive();
	*data_buffer = length;
	checksum += length;
	data_buffer++;

	char type;
	type = USART_Receive();
	*data_buffer = type;
	checksum += type;
	data_buffer++;

	// Parse first byte for msg length
	for (char i = 0; i < length; i++) {
		*data_buffer = USART_Receive();
		checksum += *data_buffer;
		data_buffer++;
	}

	// Checksum
	*data_buffer = USART_Receive();
}