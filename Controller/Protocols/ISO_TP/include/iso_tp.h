/***********************************************
*
* Implementation of the ISO 15765-2 Protocol
*
* File: iso_tp.h
*
***********************************************/

#ifndef _ISO_TP_H_
#define _ISO_TP_H_

typedef struct iso_tp_received_struct  {
	unsigned char* data;
	unsigned int length;
}ISO_TP_RECEIVED;

void init_iso_tp_layer();

/* External access to the Protocol
*
*  After successfull receive, the ISO_TP_RECEIVED* getISO_TP_struct() function returns the collected data in the returned struct
*
*  parameter: function pointer
*				1. function to initiate a new receive (returns error code)
*				2. getter for the received data array
*				3. getter for array length
*				4. send function (to send Flow Control Frame)
*
*  returns: uint8_t value
*				0x00: Success
*				0xD1: No First Frame (only for multiple frames)
*				0xD2: Frame Type not consecutive
*				0xD3: Lost Frame (frame idx wrong)
*				0xD4: More data received than expected
*			   error: Receive error (returns value returned by receive function)
*/
unsigned char iso_tp_receiveData(unsigned char (*dataLayer_receiveData)(),
	unsigned char* (*getReceivedData)(),
	unsigned char(*getReceivedDataLength)(),
	unsigned char(*sendMsg)(unsigned char* data, unsigned char nbytes));

ISO_TP_RECEIVED* getISO_TP_struct();

#endif // !_ISO_TP_H_
