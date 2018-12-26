#include <USART/include/USART.h>
#include <error_defs.h>
#include <instructionSet.h>

#include "../include/dataLayer_USART.h"
#include "../../Protocol/ApplicationLayer/include/applicationLayer.h"

void clearMessage();
unsigned char decodeMessage();
void instruction();
void regular();
void usart_send_data(unsigned char* data, unsigned char nbytes, unsigned char type);
unsigned char usart_receive_data(unsigned char* data_buffer);

void init_dataLayer_USART() {
	USART_Init_Receiver();
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

	unsigned char error = decodeMessage();

	if (error != CODE_OK) {
		reply_data[0] = error;
		reply_size = 1;
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
	usart_send_data(reply_data, reply_size,REGULAR_INSTRUCTION);
}

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
		return CODE_CHECKSUM_ERROR;
	}
	return CODE_OK;
}

void instruction() {
	unsigned char error;
	switch (msg.type) {
		case FAST_INIT_INSTRUCTION:
			error = init_diagnose(msg.data, msg.length);
			if (error = CODE_OK) {
				reply_data[0] = received_data[0];
				reply_data[1] = received_data[1];
				reply_size = 2;
			}
	}
}

void regular() {
	parseRequest(msg.data, msg.length);
	for (unsigned char i = 0; i < *received_nbytes; i++) {
		reply_data[i] = received_data[i];
	}
	reply_size = *received_nbytes;
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

unsigned char usart_receive_data(unsigned char* data_buffer) {
	set_Receiver();

	char checksum = 0;
	char length;
	length = USART_Receive();
	*data_buffer = length;
	checksum += length;
	data_buffer++;

	// Parse first byte for msg length
	for (char i = 0; i < length; i++) {
		*data_buffer = USART_Receive();
		checksum += *data_buffer;
		data_buffer++;
	}

	// Receive checksum
	unsigned char rec_checksum;
	rec_checksum = USART_Receive();
	if (checksum != rec_checksum) {
		return CODE_CHECKSUM_ERROR;
	}
	return CODE_OK;

}