#include <util/delay.h>
#include <error_defs.h>

#include "../../../LCD/lcd-routines.h"
#include "../include/applicationLayer_KWP2000.h"
#include "../../DataLayer/include/dataLayer_KWP2000.h"

void decodeError(unsigned char error);


void init_obd() {
	init_dataLayer();
}

/*
* Start Diagnose Session
* Initialize using the correct protocol
*
* Further Protocols might be tried out here
*
* returns: CODE_OK or specific error
*
*/
unsigned char init_diagnose(unsigned char* data, unsigned char nbytes){
	return start_communication_fastInit(data, nbytes);
}

unsigned char parseRequest(unsigned char* request, unsigned char nbytes) {
	return handleRequest(request, nbytes);
}