#ifndef _DATALAYER_KWP2000_H_
#define _DATALAYER_KWP2000_H_

void init_dataLayer();
void wake_up_unit();
unsigned char start_communication_fastInit(unsigned char* data, unsigned char nbytes);
unsigned char stop_communication();
unsigned char handleRequest(unsigned char* request, unsigned char nbytes);

struct incoming_dat {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[255];
	unsigned char checksum;
	unsigned char dataStream[255];
	unsigned char dataStreamLength;
}incoming;

#endif // !_DATALAYER_KWP2000_H_