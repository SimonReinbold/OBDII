#include <USART/include/USART.h>
#include <error_defs.h>

#include "../include/dataLayer_USART.h"

void clearMessage();
unsigned char decodeMessage();
void usart_send_data(unsigned char type, unsigned char* data, unsigned char nbytes);
void usart_receive_data(unsigned char* data_buffer);

void init_dataLayer_USART() {
	USART_Init_Receiver();
	reply_data[0] = 0;
	reply_size = 0;
	reply_type = CODE_OK;
}

void clearMessage() {
	msg_USART.checksum = 0;
	msg_USART.type = 0;
	msg_USART.length = 0;

	for (int i = 0; i < BUFFER_SIZE; i++) {
		msg_USART.data[i] = 0;
	}
}

unsigned char receiveAndParseUSART() {
	// Wait for instructions
	usart_receive_data(uart_buffer);

	return decodeMessage();
}

void replyUSART(unsigned char status, unsigned char* reply_data, unsigned char nbytes) {
	usart_send_data(status, reply_data, nbytes);
}

/* 
* Decodes receved USART message and assigns correspondant msg struct values
*
* return: CODE_CHECKSUM_ERROR or CODE_OK
*/
unsigned char decodeMessage() {
	clearMessage();

	msg_USART.length = uart_buffer[0];
	msg_USART.checksum += msg_USART.length;
	msg_USART.type = uart_buffer[1];
	msg_USART.checksum += msg_USART.type;

	for (int i = 0; i < msg_USART.length; i++) {
		msg_USART.data[i] = uart_buffer[i+2];
		msg_USART.checksum += msg_USART.data[i];
	}

	if (msg_USART.checksum != uart_buffer[msg_USART.length + 2]) {
		return CODE_CHECKSUM_ERROR_USART;
	}
	
	return CODE_OK;
}

void usart_send_data(unsigned char type, unsigned char* data, unsigned char nbytes) {
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