#ifndef _PROTOCOLSELECTOR_H_
#define _PROTOCOLSELECTOR_H_

void init_ProtocolSelector();
void updateProtocolSelector();
unsigned char assignRequest(unsigned char* data, unsigned char nbytes);
unsigned char* getReplyData_Protocol();
unsigned char getReplyDataLength_Protocol();

#endif // !_PROTOCOLSELECTOR_H_
