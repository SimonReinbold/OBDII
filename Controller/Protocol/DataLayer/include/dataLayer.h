void init_dataLayer();
unsigned char start_communication_fastInit(unsigned char* data, unsigned char nbytes);
unsigned char stop_communication();
unsigned char first();
unsigned char handleRequest(unsigned char* request, unsigned char nbytes);

struct {
	unsigned char format_byte;
	unsigned char target;
	unsigned char source;
	unsigned char length;
	unsigned char service_id;
	unsigned char data[255];
	unsigned char checksum;
}incoming;