#include <error_defs.h>
#include <stdlib.h>

#include "../../../include/ProtocolSelector.h"

#include "../include/SessionHandler_KWP2000.h"
#include "../include/Session_KWP2000.h"
#include "../../DataLayer/include/dataLayer_KWP2000.h"

#define MAX_SESSIONS	5

Session_KWP2000* getSessionByTargetID(unsigned char target);
Session_KWP2000* startNewSession(unsigned char target);
void parseResponse(Session_KWP2000* session);

// Active Session
Session_KWP2000** activeSessionsArray;
unsigned char activeSessions;

void init_SessionHandler_KWP2000() {
	activeSessionsArray = (Session_KWP2000**)malloc(MAX_SESSIONS * sizeof(Session_KWP2000*));
	activeSessions = 0;
	init_dataLayer();
}

void destroy_SessionHandler_KWP2000() {

}

unsigned char request_Protocol_KWP2000(unsigned char* data, unsigned char nbytes) {
	Session_KWP2000* currentSession = getSessionByTargetID(*data++); // Retreive Session with requested Unit
	unsigned char error = request_Session_KWP2000(currentSession, data, nbytes - 1); // Forward request with excluded target address
	
	// Re-Init if no data received
	if (error == CODE_NO_DATA) {
		unsigned char fast_init_SID = 0x81;
		Session_KWP2000* fastInit_Session = getSessionByTargetID(0x33);
		error = request_Session_KWP2000(fastInit_Session, &fast_init_SID, 1); // Fast Init
		if (error != CODE_OK) {
			return error;
		}
		parseResponse(fastInit_Session);
		error = request_Session_KWP2000(currentSession, data, nbytes - 1); // Forward request with excluded target address
	}

	if (error != CODE_OK) {
		return error;
	}
	parseResponse(currentSession);
	return error;
}

unsigned char* getReplyProtocol_KWP2000() {
	return getReplyProtocol_Session_KWP2000();
}

unsigned char getReplyLengthProtocol_KWP2000() {
	return getReplyLengthProtocol_Session_KWP2000();
}

Session_KWP2000* getSessionByTargetID(unsigned char target) {
	// Iterate over active Sessions array and return Session pointer if available
	for (unsigned char i = 0; i < activeSessions; i++) {
		if (activeSessionsArray[i]->target == target) {
			return activeSessionsArray[i];
		}
	}
	
	if (activeSessions < MAX_SESSIONS) {
		// If no active Session can be found, create new Session
		return startNewSession(target);
	}
	return activeSessionsArray[0]; // TODO handle max sessions
}

Session_KWP2000* startNewSession(unsigned char target) {
	// Create new Session
	Session_KWP2000* tmp = newSession(target);

	// Append Session Pointer in Session List
	activeSessionsArray[activeSessions++] = tmp;
	return tmp;
}

void parseResponse(Session_KWP2000* session) {
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