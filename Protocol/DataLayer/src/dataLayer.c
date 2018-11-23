#include <avr/io.h>
#include "../include/dataLaver.h"
#include "../../PhysicalLayer/include/physicalLayer.h"

char init_dataLayer(){
	obd_hardware_init();
}