void init_obd(void);
unsigned char init_diagnose(unsigned char* data, unsigned char nbytes);
unsigned char parseRequest(unsigned char* request, unsigned char nbytes);

unsigned char* received_data;
unsigned char* received_nbytes;