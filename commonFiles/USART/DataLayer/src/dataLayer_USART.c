#include <error_defs.h>

#include "../include/dataLayer_USART.h"
#include "../../PhysicalLayer/include/physicalLayer_USART.h"

#include "../../../../Handheld/LCD/lcd-routines.h"
#include <util/delay.h>

void clearMessage();

void init_dataLayer_USART() {
	USART_Init_Transceiver();
	clearMessage();
}

void clearMessage() {
	msg_USART.checksum = 0;
	msg_USART.type = 0;
	msg_USART.length = 0;

	for (int i = 0; i < BUFFER_SIZE; i++) {
		msg_USART.data[i] = 0;
	}
}

void usart_send_data(unsigned char type, unsigned char* data, unsigned char nbytes) {
	set_Transmitter();
	
	char checksum = 0;

	checksum += nbytes;
	USART_Transmit(nbytes);
	checksum += type;
	USART_Transmit(type);
	
	for (char i = 0; i < nbytes; i++) {
		checksum += data[i];
		USART_Transmit(data[i]);

		if (i == nbytes - 1) {
			clearTransmitCompleteFlag();
		}
	}
	enable_transmit_complete_Interrupt();
	USART_Transmit(checksum);	
}

unsigned char usart_receive_data() {
	set_Receiver();

	clearMessage();

	msg_USART.length = USART_Receive();
	msg_USART.checksum += msg_USART.length;
	
	msg_USART.type = USART_Receive();
	msg_USART.checksum += msg_USART.type;
	
	// Parse first byte for msg length
	for (unsigned char i = 0; i < msg_USART.length; i++) {
		msg_USART.data[i] = USART_Receive();
		msg_USART.checksum += msg_USART.data[i];
	}

	// Checksum
	char checksum = USART_Receive();
	if (msg_USART.checksum != checksum) {
		return CODE_CHECKSUM_ERROR_USART;
	}

	if (isSetManualRXTrigger()) {
		clearManualRXTrigger();
		return CODE_MANUAL_STOP;
	}

	return CODE_OK;
}