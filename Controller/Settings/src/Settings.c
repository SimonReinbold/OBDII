#include "../include/Settings.h"
#include "../../Protocols/include/ProtocolSelector.h"

// Smart ForTwo Cabrio
#define DEFAULT_HSN 9891
#define DEFAULT_TSN 002

void init_Settings() {
	setHSN(DEFAULT_HSN);
	setTSN(DEFAULT_TSN);
}

void changeSettings(unsigned char* data, unsigned char nbytes) {
	updateProtocolSelector();
	// TODO
}

void setHSN(int manufacturer) {
	HSN = manufacturer;
}

void setTSN(int type) {
	TSN = type;
}