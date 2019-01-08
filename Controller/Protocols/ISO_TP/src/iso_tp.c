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
}

void init_iso_tp_layer() {
	struct_ptr = (ISO_TP_RECEIVED*)malloc(sizeof(ISO_TP_RECEIVED));
	struct_ptr->length = 0;
	struct_ptr->data = (unsigned char*)malloc(struct_ptr->length * sizeof(unsigned char));
}

unsigned char iso_tp_receiveData(unsigned char (*dataLayer_receiveData)(),
	unsigned char* (*getReceivedData)(),
	unsigned char (*getReceivedDataLength)(),
	unsigned char (*sendMsg)(unsigned char* data, unsigned char nbytes)){
	
	unsigned char error = dataLayer_receiveData();
	if (error != 0)								return error;	// Return on error

	unsigned char* frame = getReceivedData();
	ISO_TP_RECEIVED* struct_out = getISO_TP_struct();
	resetISO_TP_struct();

	// First message received, check for type
	if (frame[0] >> 4 == 0) {
		// Single Frame
		struct_out->length = frame[0] & 0x0F;
		struct_out->data = (unsigned char*) realloc(struct_out->data, struct_out->length);
		for (unsigned char i = 0; i < struct_out->length; i++) {
			struct_out->data[i] = frame[i + 1];
		}
	}
	else {
		// Multiple Frames, first received
		if (frame[0] >> 4 != 1)					return 0xD1;	// No First Frame

		struct_out->length = 256*(frame[0] & 0x0F) + frame[1];	// 12 bit size -> store in int
		unsigned int received_size = 0;

		for (unsigned int i = 2; i < getReceivedDataLength(); i++) {
			struct_out->data[received_size++] = frame[i];
		}

		// Reply with flow control frame
		unsigned char flowControl[3];
		flowControl[0] = 0x30 | 0x00;						// Type 3 and continue to send 0x00
		flowControl[1] = 0;									// No stop between frames
		flowControl[3] = 50;								// Separation time in ms
		sendMsg(flowControl, 3);

		// Received rest of data in several frames.
		unsigned char frame_idx = 0;
		while (received_size < struct_out->length) {

			error = dataLayer_receiveData();
			if (error != 0)						return error;	// Return on error

			frame = getReceivedData();						// Pointer should not change, but just to be safe request it again

			if ((frame[0] & 0xF0) != 2)			return 0xD2;	// Not a consecutive frame
			if ((frame[0] & 0x0F) != frame_idx) return 0xD3;	// expected different frame index

			frame_idx = (frame_idx + 1) % 16;				// Count from 0 to 15 and then loop back to 0

			for (unsigned int i = 1; i < getReceivedDataLength(); i++) {
				struct_out->data[received_size++] = frame[i];
			}
		}
		// Receive finished
		if (received_size > struct_out->length)	return 0xD4;	// More data received than expected
	}
	return 0;												// Receive successful
}