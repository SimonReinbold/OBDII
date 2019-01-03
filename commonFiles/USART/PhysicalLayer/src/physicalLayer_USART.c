#include <avr/io.h>
#include <avr/interrupt.h>
#include "../include/physicalLayer_USART.h"

volatile unsigned char manualRXTrigger;

void setManualRXTrigger() {
	manualRXTrigger = 1;
}

void clearManualRXTrigger() {
	manualRXTrigger = 0;
}

unsigned char isSetManualRXTrigger() {
	return manualRXTrigger;
}

void USART_Init_Receiver() {
	USART_Init();
	set_Receiver();
}

void USART_Init_Transmitter() {
	USART_Init();
	set_Transmitter();
}

void USART_Init_Transceiver() {
	USART_Init();
	set_Tranceiver();
}

void USART_Init()
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(MYUBRR >>8);
	UBRR0L = (unsigned char)MYUBRR;

	// Enable internal Pull-ups
	PORTD |= 1 << PD0;
	PORTD |= 1 << PD1;


	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	disable_interrupts();
	clearManualRXTrigger();
}

void set_Receiver() {
	UCSR0B |= 1 << RXEN0;
	UCSR0B &= ~(1 << TXEN0);
}

void set_Transmitter() {
	UCSR0B |= 1 << TXEN0;
	UCSR0B &= ~(1 << RXEN0);
}

void set_Tranceiver() {
	UCSR0B |= (1 << RXEN0) || (1 << TXEN0);
}

void disable_USART() {
	UCSR0B &= ~(1 << TXEN0);
	UCSR0B &= ~(1 << RXEN0);
}

void disable_interrupts() {
	UCSR0B &= ~(1 << RXCIE0);
	UCSR0B &= ~(1 << TXCIE0);
	UCSR0B &= ~(1 << UDRIE0);
}

void enable_transmit_complete_Interrupt() {
	UCSR0B |= 1 << TXCIE0;
}

void enable_receive_complete_Interrupt() {
	UCSR0B |= 1 << RXCIE0;
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1 << RXC0)) && !(isSetManualRXTrigger()) );

	/* Get and return received data from buffer */
	return UDR0;
}

void clearTransmitCompleteFlag() {
	UCSR0A |= 1 << TXC0;
}

unsigned char checkTransmitComplete() {

}

ISR(USART_TX_vect) {
	// Last byte transmission complete
	// configure as receiver to wait for incoming data
	disable_interrupts();
	set_Receiver();
}