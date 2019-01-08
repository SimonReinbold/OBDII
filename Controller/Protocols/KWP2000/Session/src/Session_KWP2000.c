#include <stdlib.h>
#include <error_defs.h>

#include "../include/Session_KWP2000.h"
#include "../include/SessionHandler_KWP2000.h"
#include "../../DataLayer/include/dataLayer_KWP2000.h"

#include "../../../ISO_TP/include/iso_tp.h"

#define TESTER_ADDRESS	0xF1;

unsigned char* resetSupportedRequestArray(unsigned char* arr, unsigned char size);

unsigned char request_Session_KWP2000(unsigned char* data, unsigned char nbytes);
unsigned char* prependHeader(Session_KWP2000* session, unsigned char* data, unsigned char nbytes);
unsigned char getHeaderSize(Session_KWP2000* session, unsigned char* data, unsigned char nbytes);

unsigned char isSupportedSID(Session_KWP2000* session, unsigned char SID);
unsigned char hasPID(Session_KWP2000* session, unsigned char SID);
unsigned char isSupportedPID(Session_KWP2000* session, unsigned char SID, unsigned char PID);

Session_KWP2000* activeSession;

void setActiveSession(Session_KWP2000* currentSession) {
	activeSession = currentSession;
}

Session_KWP2000* getActiveSession() {
	return activeSession;
}

Session_KWP2000* newSession(unsigned char target) {
	Session_KWP2000* tmp = (Session_KWP2000*)malloc(sizeof(Session_KWP2000));
	tmp->target = target;
	tmp->keybytes[0] = 0x00;
	tmp->keybytes[1] = 0x00;
	tmp->requestSID = 0x00;
	tmp->requestData = (unsigned char*)malloc(0 * sizeof(unsigned char));
	tmp->requestDataLength = 0;

	switch (target){
		case 0x33:
			tmp->headerType = 0xC0;
			break;
		default:
			tmp->headerType = 0x80;
			break;
	}

	// Reset supported Requests
	resetSupportedRequestArray(tmp->supportedRequests.SID01, sizeof(tmp->supportedRequests.SID01));
	resetSupportedRequestArray(tmp->supportedRequests.SID02, sizeof(tmp->supportedRequests.SID02));
	tmp->supportedRequests.SID03 = 1;
	tmp->supportedRequests.SID04 = 1;
	resetSupportedRequestArray(tmp->supportedRequests.SID05, sizeof(tmp->supportedRequests.SID05));
	tmp->supportedRequests.SID06 = 0;
	tmp->supportedRequests.SID07 = 0;
	tmp->supportedRequests.SID08 = 0;
	resetSupportedRequestArray(tmp->supportedRequests.SID09, sizeof(tmp->supportedRequests.SID09));
	tmp->supportedRequests.SID0A = 0;
	tmp->supportedRequests.SID10 = 0;

	return tmp;
}

unsigned char* resetSupportedRequestArray(unsigned char* arr, unsigned char size) {
	for (unsigned char i = 0; i < size; i++) {
		if (i == 0) {
			arr[i] = 0x80; // Support first request to check for available PIDs
		}
		else {
			arr[i] = 0x00;
		}
	}
	return arr;
}

void destroySession() {
	//TODO
}



unsigned char handle_session_request(unsigned char* data, unsigned char nbytes) {
	// Send message
	unsigned char error = request_Session_KWP2000(data, nbytes);
	if (error != CODE_OK) {
		return error;
	}
	
	if (getActiveSession()->requestSID == 0x03) {
		//error = receive_msg();
		error = iso_tp_receiveData(receive_msg_returnArray, getIncomingDataLength, request_Session_KWP2000);
		ISO_TP_RECEIVED* iso_tp_rec = getISO_TP_struct();
		if (error != 0 || iso_tp_rec->length > MAX_DATA_LENGTH) return CODE_ISO_TP_ERROR; // Dynamic length not implemented

		// Set received Data to the incoming struct
		for (int i = 0; i < iso_tp_rec->length; i++) {
			incoming.data[i] = iso_tp_rec->data[0];
		}
		incoming.length = iso_tp_rec->length;
	}
	else {
		error = receive_msg();
	}
	return error;
}

unsigned char request_Session_KWP2000(unsigned char* data, unsigned char nbytes) {
	Session_KWP2000* session = getActiveSession();
	session->requestData = (unsigned char*)realloc(session->requestData, nbytes * sizeof(unsigned char));
	for (unsigned char i = 0; i < nbytes; i++) {
		session->requestData[i] = data[i];
	}
	session->requestDataLength = nbytes;

	unsigned char* msg = prependHeader(session, data, nbytes);

	if (session->requestSID == 0x81) {
		// Wake up pattern for Fast Init
		wake_up_unit();
	}
	
	unsigned char error = send_msg(msg, (getHeaderSize(session, data, nbytes) + 1 + nbytes));

	free(msg);
	return error;
}

unsigned char* prependHeader(Session_KWP2000* session, unsigned char* data, unsigned char nbytes) {
	unsigned char header_size = getHeaderSize(session, data, nbytes);

	// Build full header
	unsigned char* msg = (unsigned char*)malloc(header_size + 1 + nbytes);
	unsigned char* msgPtr = msg;

	*msgPtr = session->headerType;
	if (header_size == 1 || header_size == 3) *msgPtr++ |= (nbytes + 1); // Append length information in header

	if (header_size == 3 || header_size == 4) {
		*msgPtr++ = session->target;
		*msgPtr++ = TESTER_ADDRESS;
	}

	if (header_size == 2 || header_size == 4) *msgPtr++ = (nbytes + 1);

	// Append SID
	*msgPtr++ = session->requestSID;

	// Append data
	for (unsigned char i = 0; i < nbytes; i++) {
		*msgPtr++ = data[i];
	}

	return msg;
}

unsigned char getHeaderSize(Session_KWP2000* session, unsigned char* data, unsigned char nbytes) {
	// Supported action is requested build header
	unsigned char header_size = 1; // Format Byte

	if (session->requestSID != 0x81) {
		// Normal request
		if (session->keybytes[1] & 0x08)						header_size += 2;				// Address Info
		if (!(session->keybytes[1] & 0x01) || nbytes > 62)		header_size += 1;				// Length Byte
	}
	else {
		// First message -> startCommunication request without length byte
		header_size = 3;
	}
	return header_size;
}

unsigned char checkSupport(unsigned char* data, unsigned char nbytes) {
	Session_KWP2000* session = getActiveSession();
	// Check if requested SID is supported
	if (!isSupportedSID(session, session->requestSID)) {
		return CODE_UNSUPPORTED_SID;
	}

	if (nbytes > 0) {
		// Check if PID is applicable
		if (hasPID(session, session->requestSID)) {
			// PID required, check if supported
			unsigned char isSupport = isSupportedPID(session, session->requestSID, data[0]);
			if (isSupport == CODE_NONEXISTENT_PID || isSupport == 0x00) {
				// PID not supported or nonexistent, return error
				return isSupport;
			}
		}
	}

	if (session->requestSID != 0x81) {
		if (!(session->keybytes[1] & 0x0E))						return CODE_KEYBYTE_ERROR;		// No header type supported ?! Error
		if (!(session->keybytes[1] & 0x03))						return CODE_KEYBYTE_ERROR;		// No length information supported ?! Error
		if (!(session->keybytes[1] & 0x02) && nbytes > 63)		return CODE_NOT_IMPLEMENTED;	// Message too big for single request and length byte not suported
	}

	return CODE_OK;
}

unsigned char isSupportedSID(Session_KWP2000* session, unsigned char SID) {
	if (SID == 0x01 || SID == 0x02 || SID == 0x05 || SID == 0x09) {
		return 1;
	}
	else {
		switch (SID)
		{
		case 0x03:
			return session->supportedRequests.SID03;
			break;
		case 0x04:
			return session->supportedRequests.SID04;
			break;
		case 0x06:
			return session->supportedRequests.SID06;
			break;
		case 0x07:
			return session->supportedRequests.SID07;
			break;
		case 0x08:
			return session->supportedRequests.SID08;
			break;
		case 0x0A:
			return session->supportedRequests.SID0A;
			break;
		case 0x10:
			return session->supportedRequests.SID10;
			break;
		}
	}
}

unsigned char hasPID(Session_KWP2000* session, unsigned char SID) {
	if (SID == 1 || SID == 2 || SID == 5 || SID == 9) {
		return 1;
	}
	return 0;
}

unsigned char isSupportedPID(Session_KWP2000* session, unsigned char SID, unsigned char PID) {
	unsigned char byte_idx = PID / (unsigned char)8;
	unsigned char bit_idx = 8 - (PID % 8);

	switch (SID)
	{
	case 1:
		if (sizeof(session->supportedRequests.SID01) - 1 < byte_idx) {
			return CODE_NONEXISTENT_PID;
		}
		return session->supportedRequests.SID01[byte_idx] & (1 << bit_idx);
	case 2:
		if (sizeof(session->supportedRequests.SID02) - 1 < byte_idx) {
			return CODE_NONEXISTENT_PID;
		}
		return session->supportedRequests.SID02[byte_idx] & (1 << bit_idx);
	case 5:
		if (sizeof(session->supportedRequests.SID05) - 1 < byte_idx) {
			return CODE_NONEXISTENT_PID;
		}
		return session->supportedRequests.SID05[byte_idx] & (1 << bit_idx);
	case 9:
		if (sizeof(session->supportedRequests.SID09) - 1 < byte_idx) {
			return CODE_NONEXISTENT_PID;
		}
		return session->supportedRequests.SID09[byte_idx] & (1 << bit_idx);
	}
}

unsigned char* getReplyProtocol_Session_KWP2000() {
	unsigned char* ptr = incoming.dataStream;
	return ptr;
}

unsigned char getReplyLengthProtocol_Session_KWP2000() {
	return incoming.dataStreamLength;
}