#ifndef _DATALAYER_KWP2000_H_
#define _DATALAYER_KWP2000_H_

#define MAX_DATA_SIZE	254

#define ID_NEGATIVE_RESPONSE	0x7F
#define ID_NO_SUPPORT1			0x11
#define ID_NO_SUPPORT2			0x12
#define ID_NO_SUPPORT3			0x7E
#define ID_NO_SUPPORT4			0x7F

void init_dataLayer();
void wake_up_unit();
unsigned char start_communication_fastInit(unsigned char* data, unsigned char nbytes);
unsigned char stop_communication();
unsigned char receive_msg();
unsigned char send_msg(unsigned char *data, unsigned char n_bytes); 
unsigned char* getIncomingData();
unsigned char getIncomingDataLength();

struct incoming_dat {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char header_size;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[MAX_DATA_SIZE];
	unsigned char checksum;
	unsigned char dataStream[MAX_DATA_SIZE + 6];
	unsigned char dataStreamLength;
}incoming;

#endif // !_DATALAYER_KWP2000_H_