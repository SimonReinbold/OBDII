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

#include "../../Settings/include/Settings.h"

#include "../../USART/DataLayer/include/dataLayer_USART.h"

#include "../../Protocols/include/ProtocolSelector.h"

#ifndef NULL
#define NULL (void*)0
#endif // !NULL

void boot();

int main() {
	boot();

	while (1) {
		// Wait for Handheld request
		unsigned char status = receiveAndParseUSART();
		if (status != CODE_OK); {
			// Error in USART message, reply with error
			replyUSART(status, NULL, 0);
			continue;
		}

		if (msg_USART.type != REQUEST) {
			changeSettings(msg_USART.data, msg_USART.length);
			replyUSART(CODE_OK, NULL, 0);
			continue;
		}
		else {
			// Regular OBD request forward to Session
			status = assignRequest(msg_USART.data, msg_USART.length);
			if (status != CODE_OK) {
				replyUSART(status, NULL, 0);
			}
			
			// Transmit OBD reply
			replyUSART(CODE_OK,getReplyData_Protocol(), getReplyDataLength_Protocol());
		}
	}
}

void boot() {
	init_Settings();
	init_ProtocolSelector();
	init_dataLayer_USART();

	// Debug LED 
	DDRD |= 1 << PD7;
	PORTD &= ~(1 << PD7);
}