/***********************************************
*
* Implementation of the ISO 15765-2 Protocol
*
* File: iso_tp.c
*
***********************************************/

#ifndef NULL
#define NULL (void*)0
#endif // !NULL

#include <stdlib.h>

#include "..\include\iso_tp.h"

void resetISO_TP_struct();

ISO_TP_RECEIVED* struct_ptr;

ISO_TP_RECEIVED* getISO_TP_struct() {
	return struct_ptr;
}

void resetISO_TP_struct() {
	ISO_TP_RECEIVED* struct_out = getISO_TP_struct();
	struct_out->length = 0;
	struct_out->error = 0;
}

void init_iso_tp_layer() {
	struct_ptr = (ISO_TP_RECEIVED*)malloc(sizeof(ISO_TP_RECEIVED));
	struct_ptr->error = 0;
	struct_ptr->length = 0;
	struct_ptr->data = (unsigned char*)realloc(struct_ptr->data, struct_ptr->length);
}

unsigned char iso_tp_receiveData(unsigned char *(*dataLayer_receiveData)(),
	unsigned char (*getReceivedDataLength)(),
	unsigned char (*sendMsg)(unsigned char* data, unsigned char nbytes)){

	unsigned char* frame = dataLayer_receiveData();
	ISO_TP_RECEIVED* struct_out = getISO_TP_struct();
	resetISO_TP_struct();

	// First message received, check for type
	if (*frame >> 4 == 0) {
		// Single Frame
		struct_out->length = *frame & 0x0F;
		struct_out->data = (unsigned char*) realloc(struct_out->data, struct_out->length);
		for (unsigned char i = 0; i < struct_out->length; i++) {
			struct_out->data[i] = frame[i + 1];
		}
	}
	else {
		// Multiple Frames, first received
		if (*frame >> 4 != 1) {} //TODO Error handling for wrong frame type

		struct_out->length = (8 << (*frame & 0x0F)) | (frame[1]);	// 12 bit size -> store in int
		unsigned int received_size = 0;

		for (unsigned int i = 0; i < getReceivedDataLength() - 2; i++) {
			struct_out->data[received_size] = frame[i + 1];
			received_size++;
		}

		// Reply with flow control frame
		unsigned char flowControl[3];
		flowControl[0] = 0x30 | 0x00;	// Type 3 and continue to send 0x00
		flowControl[1] = 0;				// No stop between frames
		flowControl[3] = 50;			// Separation time in ms
		sendMsg(flowControl, 3);

		// Received rest of data in several frames.
		unsigned char frame_idx = 0;
		while (received_size < struct_out->length) {
			frame = dataLayer_receiveData();

			if ((*frame & 0xF0) != 2) {} // TODO Error handling for wrong frame type
			if ((*frame & 0x0F) != frame_idx) {} // TODO Error handling for missing frame

			frame_idx = (frame_idx + 1) % 16; // Count from 0 to 15

			for (unsigned int i = 0; i < getReceivedDataLength() - 1; i++) {
				struct_out->data[received_size] = frame[i + 1];
				received_size++;
			}
		}
		// Receive finished
		if (received_size > struct_out->length) {} // TODO Error handling too many bytes received
	}
	return 0;
}