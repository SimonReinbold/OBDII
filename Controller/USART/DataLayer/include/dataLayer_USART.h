#ifndef _DECODEUSART_H_
#define _DECODEUSART_H_

#define BUFFER_SIZE		255
#define REPLY_SIZE		20

unsigned char uart_buffer[BUFFER_SIZE];
unsigned char reply_data[REPLY_SIZE];
unsigned char reply_size;
unsigned char reply_type;

struct decodedMessage {
	unsigned char type;
	unsigned char length;
	unsigned char data[BUFFER_SIZE];
	unsigned char checksum;
} msg;

void init_dataLayer_USART();
void reply();
void executeRequest();

#endif //!_DECODEUSART_H_