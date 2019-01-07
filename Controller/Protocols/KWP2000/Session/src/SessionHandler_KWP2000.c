#include <error_defs.h>
#include <stdlib.h>

#include "../../../include/ProtocolSelector.h"

#include "../include/SessionHandler_KWP2000.h"
#include "../include/Session_KWP2000.h"
#include "../../DataLayer/include/dataLayer_KWP2000.h"

#include "../../../ISO_TP/include/iso_tp.h"

#define MAX_SESSIONS	5

Session_KWP2000* getSessionByTargetID(unsigned char target);
Session_KWP2000* startNewSession(unsigned char target);
void parseResponse();

// Active Session
Session_KWP2000** activeSessionsArray;
unsigned char activeSessions;

void init_SessionHandler_KWP2000() {
	activeSessionsArray = (Session_KWP2000**)malloc(MAX_SESSIONS * sizeof(Session_KWP2000*));
	activeSessions = 0;
	init_dataLayer();
	init_iso_tp_layer();
}

void destroy_SessionHandler_KWP2000() {

}

unsigned char request_Protocol_KWP2000(unsigned char* data, unsigned char nbytes) {
	unsigned char targetID = *data++;
	Session_KWP2000* session = getSessionByTargetID(targetID);

	setActiveSession(session);					// Set active session by target ID
	session->requestSID = *data++;

	// Parse input for error
	if (nbytes < 1) {
		return CODE_NO_REQUEST_DATA;
	}
	if (nbytes > 256) {
		return CODE_DATA_ERROR;
	}

	unsigned char error = checkSupport(data, nbytes - 2);
	if (error != CODE_OK) {
		return error;
	}

	error = handle_session_request(data, nbytes - 2); // Forward request with excluded target and sid
	
	// Re-Init if no data received
	if (error == CODE_NO_DATA) {
		Session_KWP2000* fastInitSession = getSessionByTargetID(0x33);
		fastInitSession->requestSID = 0x81;
		setActiveSession(fastInitSession);
		error = handle_session_request(NULL, 0); // Fast Init
		if (error != CODE_OK) {
			return error;
		}
		parseResponse();
		setActiveSession(session);
		error = handle_session_request(data, nbytes - 2); // Forward request with excluded target address
	}

	if (error != CODE_OK) {
		return error;
	}
	parseResponse();
	return error;
}

unsigned char* getReplyProtocol_KWP2000() {
	return getReplyProtocol_Session_KWP2000();
}

unsigned char getReplyLengthProtocol_KWP2000() {
	return getReplyLengthProtocol_Session_KWP2000();
}

Session_KWP2000* getSessionByTargetID(unsigned char target) {
	Session_KWP2000* tmp;
	// Iterate over active Sessions array and return Session pointer if available
	for (unsigned char i = 0; i < activeSessions; i++) {
		if (activeSessionsArray[i]->target == target) {
			tmp = activeSessionsArray[i];
			return tmp;
		}
	}
	
	if (activeSessions < MAX_SESSIONS) {
		// If no active Session can be found, create new Session
		tmp = startNewSession(target);
	}
	else {
		tmp = activeSessionsArray[0]; // TODO handle max sessions
	}
	return tmp;
}

Session_KWP2000* startNewSession(unsigned char target) {
	// Create new Session
	Session_KWP2000* tmp = newSession(target);

	// Append Session Pointer in Session List
	activeSessionsArray[activeSessions++] = tmp;
	return tmp;
}

void parseResponse(){
	Session_KWP2000* session = getActiveSession();
	unsigned char idx;
	switch (incoming.service_id) {
	case 0x7F:
		// Negative response, do nothing
		break;
	case 0xC1:
		// Fast Init response
		session->keybytes[1] = incoming.data[0];
		session->keybytes[0] = incoming.data[1];
		// Set keybytes of source address session
		Session_KWP2000* response_Session = getSessionByTargetID(incoming.source);
		response_Session->keybytes[1] = incoming.data[0];
		response_Session->keybytes[0] = incoming.data[1];
		break;
	case 0x41:
		if (incoming.data[0] % 0x20 != 0) return; // Not a supported PID request
		idx = incoming.data[0] / (unsigned char)8;
		session->supportedRequests.SID01[0] = incoming.data[idx++];
		session->supportedRequests.SID01[1] = incoming.data[idx++];
		session->supportedRequests.SID01[2] = incoming.data[idx++];
		session->supportedRequests.SID01[3] = incoming.data[idx];
		break;
	default:
		break;
	}
}