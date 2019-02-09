/********************************************
*
* Interface for associated MenuItem
* functions declaration
*
********************************************/

void init_applicationLayer_KWP2000();
unsigned char start_communication_fastInit();
unsigned char requestPIDs();

// SID 0x01, PID 0x04
unsigned char calcEngineLoad();
// SID 0x01, PID 0x05
unsigned char engineCoolantTemp();
// SID 0x01, PID 0x0B
unsigned char intakeManifoldAbsolutePressure();
// SID 0x01, PID 0x0C
unsigned char engineRPM();
// SID 0x01, PID 0x0D
unsigned char vehicleSpeed();
// SID 0x01, PID 0x0F
unsigned char intake_air_Temp();
// SID 0x01, PID 0x10
unsigned char mafAirFlowRate();
// SID 0x01, PID 0x11
unsigned char throttlePosition();

unsigned char stop_communication();
unsigned char requestDTCs();
unsigned char clearDTCs();
unsigned char requestPermDTCs();