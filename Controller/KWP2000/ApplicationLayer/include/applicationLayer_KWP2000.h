#ifndef _APPLICATIONLAYER_KWP2000_H_
#define _APPLICATIONLAYER_KWP2000_H_

void init_obd(void);
unsigned char init_diagnose(unsigned char* data, unsigned char nbytes);
unsigned char parseRequest(unsigned char* request, unsigned char nbytes);

#endif // !_APPLICATIONLAYER_KWP2000_H_