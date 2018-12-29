/********************************************
*
* Implements concrete commands passed
* to the controller
*
********************************************/

#include <instructionSet.h>
#include <error_defs.h>
#include <USART/include/USART.h>

#include "../include/dataLayer_USART.h"

void usart_send_data(unsigned char* data, unsigned char nbytes, unsigned char type);
void clearMessage();

void init_dataLayer() {
	USART_Init_Transceiver();
}

void usart_send_instruction(unsigned char* data, unsigned char nbytes, unsigned char instruction) {
	usart_send_data(data, nbytes, instruction);
}

void usart_send_command(unsigned char* data, unsigned char nbytes) {
	usart_send_data(data, nbytes, REGULAR_INSTRUCTION);
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

unsigned char usart_receive_data() {
	clearMessage();
	set_Receiver();

	char checksum = 0;
	msg.length = USART_Receive();
	checksum += msg.length;

	msg.type = USART_Receive();
	checksum += msg.type;

	// Parse first byte for msg length
	for (char i = 0; i < msg.length; i++) {
		msg.data[i] = USART_Receive();
		checksum += msg.data[i];
	}

	// Checksum
	msg.checksum = USART_Receive();

	if (msg.checksum != checksum) {
		return CODE_CHECKSUM_ERROR_USART;
	}

	return CODE_OK;
}

void clearMessage() {
	msg.checksum = 0;
	msg.type = 0;
	msg.length = 0;

	for (int i = 0; i < BUFFER_SIZE; i++) {
		msg.data[i] = 0;
	}
}