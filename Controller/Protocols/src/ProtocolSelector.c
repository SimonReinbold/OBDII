#include "../include/ProtocolSelector.h"

// Include SessionHandlers of available Protocols
#include "../KWP2000/Session/include/SessionHandler_KWP2000.h"

unsigned char (*protocol_request)(unsigned char*, unsigned char);
unsigned char* (*protocol_data)();
unsigned char (*protocol_dataLength)();
void(*init_Protocol)();
void(*destroy_Protocol)();

void init_ProtocolSelector() {
	// Default Protocol KWP2000
	init_Protocol = &init_SessionHandler_KWP2000;
	destroy_Protocol = &destroy_SessionHandler_KWP2000;
	protocol_request = &request_Protocol_KWP2000;
	protocol_data = &getReplyProtocol_KWP2000;
	protocol_dataLength = &getReplyLengthProtocol_KWP2000;

	init_Protocol();
}

void updateProtocolSelector() {
	// TODO
	// Check if protocol has changed, if so destroy old SessionHandler and init new
}

unsigned char assignRequest(unsigned char* data, unsigned char nbytes) {
	return protocol_request(data, nbytes); // Function pointer to selected protocol based on defined settings
}

unsigned char* getReplyData_Protocol(){
	return protocol_data();
}

unsigned char getReplyDataLength_Protocol(){
	return protocol_dataLength();
}