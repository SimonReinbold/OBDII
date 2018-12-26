/********************************************
*
* Implements concrete commands passed
* to the controller
*
********************************************/

#include <instructionSet.h>
#include <error_defs.h>
#include <USART/include/USART.h>

void usart_send_data(unsigned char* data, unsigned char nbytes, unsigned char type);
unsigned char usart_receive_data(unsigned char* data_buffer);

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