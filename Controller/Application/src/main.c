/**************************************************************
*
*
*                        CONTROLLER
*
*
***************************************************************/
#include <util/delay.h>
#include <avr/io.h>
#include <error_defs.h>
#include <instructionSet.h>
#include <USART/DataLayer/include/dataLayer_USART.h>

#include "../../Settings/include/Settings.h"
#include "../../Protocols/include/ProtocolSelector.h"

void boot();

int main() {
	boot();

	while (1) {
		// Wait for Handheld request
		unsigned char status = usart_receive_data();

		if (status != CODE_OK && msg_USART.type != REQUEST){
			// Error in USART message, reply with error
			usart_send_data(status, NULL, 0);
			continue;
		}
		if (msg_USART.type != REQUEST) {
			changeSettings(msg_USART.data, msg_USART.length);
			usart_send_data(CODE_OK, NULL, 0);
			continue;
		}
		else {
			// Regular OBD request forward to Session
			status = assignRequest(msg_USART.data, msg_USART.length);
			
			usart_send_data(status, getReplyData_Protocol(), getReplyDataLength_Protocol());
			continue;
		}
	}
}

void boot() {
	// Debug LED 
	DDRD |= 1 << PD7;
	PORTD &= ~(1 << PD7);

	init_Settings();
	init_ProtocolSelector();
	init_dataLayer_USART();
}