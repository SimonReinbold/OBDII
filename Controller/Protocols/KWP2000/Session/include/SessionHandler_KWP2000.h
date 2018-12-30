#ifndef _SESSIONHANDLER_KWP2000_H
#define _SESSIONHANDLER_KWP2000_H

void init_SessionHandler_KWP2000();
void destroy_SessionHandler_KWP2000();
unsigned char request_Protocol_KWP2000(unsigned char* data, unsigned char nbytes);
unsigned char* getReplyProtocol_KWP2000();
unsigned char getReplyLengthProtocol_KWP2000();

#endif // !_SESSIONHANDLER_KWP2000_H
