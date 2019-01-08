#include <stdint.h>

#define MYUBRR (uint16_t)(F_CPU/16/SERIAL_BAUD-1)

void USART_Init_Receiver();

void USART_Init_Transmitter();

void USART_Init_Transceiver();

void USART_Init();

void set_Receiver();

void set_Transmitter();

void set_Tranceiver();

void disable_USART();

void disable_interrupts();

void enable_transmit_complete_Interrupt();

void enable_receive_complete_Interrupt();

void USART_Transmit(unsigned char data);

unsigned char USART_Receive(void);

void clearTransmitCompleteFlag();

unsigned char checkTransmitComplete();

void setManualRXTrigger();

void clearManualRXTrigger();

unsigned char isSetManualRXTrigger();