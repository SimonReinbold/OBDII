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
	unsigned char error;
}ISO_TP_RECEIVED;

void init_iso_tp_layer();

/* External access to the Protocol
*
*  parameter: function pointer to lower layer data receive, length, send data and bitrate
*  returns: pointer to full data structure
*/
unsigned char iso_tp_receiveData(unsigned char *(*dataLayer_receiveData)(),
	unsigned char(*getReceivedDataLength)(),
	unsigned char(*sendMsg)(unsigned char* data, unsigned char nbytes));

ISO_TP_RECEIVED* getISO_TP_struct();

#endif // !_ISO_TP_H_
