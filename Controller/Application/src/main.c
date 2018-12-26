/**************************************************************
*
*
*                        CONTROLLER
*
*
***************************************************************/

#include "../include/dataLayer_USART.h"
#include "../../Protocol/ApplicationLayer/include/applicationLayer.h"

void boot();

int main() {
	boot();
	
	while (1) {
		// Execute request
		executeRequest();

		// Transmit reply
		reply();
	}
}

void boot() {
	init_obd();
	init_dataLayer_USART();
}