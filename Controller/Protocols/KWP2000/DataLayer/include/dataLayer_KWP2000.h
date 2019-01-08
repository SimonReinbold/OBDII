#ifndef _DATALAYER_KWP2000_H_
#define _DATALAYER_KWP2000_H_

#define MAX_DATA_LENGTH		255

void init_dataLayer();
void wake_up_unit();
unsigned char start_communication_fastInit(unsigned char* data, unsigned char nbytes);
unsigned char stop_communication();
unsigned char receive_msg();
unsigned char send_msg(unsigned char *data, unsigned char n_bytes); 
unsigned char* receive_msg_returnArray();
unsigned char getIncomingDataLength();

struct incoming_dat {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[MAX_DATA_LENGTH];
	unsigned char checksum;
	unsigned char dataStream[MAX_DATA_LENGTH + 5];
	unsigned char dataStreamLength;
}incoming;

#endif // !_DATALAYER_KWP2000_H_