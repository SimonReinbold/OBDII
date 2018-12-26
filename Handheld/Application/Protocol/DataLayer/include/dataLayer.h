/********************************************
*
* Interface for low level executables
*
********************************************/

void init_dataLayer();

/* Instructions are used if an action requires special communication
* E.g. Fast init requires a wake up pattern before the message is sent
* Compare instructionSet.h for available instructions
*/
void usart_send_instruction(unsigned char* data, unsigned char nbytes, unsigned char instruction);

// Commands directly cause the request beeing transferred to the receiver
void usart_send_command(unsigned char* data, unsigned char nbytes);

unsigned char usart_receive_data(unsigned char* data_buffer);