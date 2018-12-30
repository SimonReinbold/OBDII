#include <error_defs.h>
#include <stdlib.h>

#include "../../../include/ProtocolSelector.h"

#include "../include/SessionHandler_KWP2000.h"
#include "../include/Session_KWP2000.h"

#define MAX_SESSIONS	10

Session_KWP2000* getSessionByTargetID(unsigned char target);
Session_KWP2000* startNewSession(unsigned char target);

// Active Session
Session_KWP2000** activeSessionsArray;
unsigned char activeSessions;

void init_SessionHandler_KWP2000() {
	*activeSessionsArray = (Session_KWP2000*)malloc(MAX_SESSIONS * sizeof(Session_KWP2000*));
	activeSessions = 0;
}

void destroy_SessionHandler_KWP2000() {

}

unsigned char request_Protocol_KWP2000(unsigned char* data, unsigned char nbytes) {
	Session_KWP2000* currentSession = getSessionByTargetID(*data++); // Retreive Session with requested Unit
	unsigned char error = request_Session_KWP2000(currentSession, data, nbytes - 1); // Forward request with excluded target address
	if (error != CODE_OK) {
		return error;
	}
}

unsigned char* getReplyProtocol_KWP2000() {
	return getReplyProtocol_Session_KWP2000();
}

unsigned char getReplyLengthProtocol_KWP2000() {
	return getReplyLengthProtocol_Session_KWP2000();
}

Session_KWP2000* getSessionByTargetID(unsigned char target) {
	// Iterate over active Sessions array and return Session pointer if available
	for (unsigned char i = 0; i < MAX_SESSIONS; i++) {
		if (activeSessionsArray[i]->target == target) {
			return activeSessionsArray[i];
		}
	}

	// If no active Session can be found, create new Session
	return startNewSession(target);
}

Session_KWP2000* startNewSession(unsigned char target) {
	// Create new Session
	Session_KWP2000* tmp = newSession(target);

	// Append Session Pointer in Session List
	activeSessionsArray[activeSessions] = tmp;
	activeSessions++;
}