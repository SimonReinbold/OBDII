#ifndef _DECODEUSART_H_
#define _DECODEUSART_H_

#define BUFFER_SIZE		255

struct decodedMessage {
	unsigned char type;
	unsigned char length;
	unsigned char data[BUFFER_SIZE];
	char checksum;
}msg_USART;

void init_dataLayer_USART();
void usart_send_data(unsigned char type, unsigned char* data, unsigned char nbytes);
unsigned char usart_receive_data();

#endif //!_DECODEUSART_H_