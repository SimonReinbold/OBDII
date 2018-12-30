#ifndef _SETTINGS_H_
#define _SETTINGS_H_

int HSN; // HSN(manufacturer key number) 
int TSN; // TSN(type key number)

void init_Settings();
void changeSettings(unsigned char* data, unsigned char nbytes);
void setHSN(int manufacturer);
void setTSN(int type);

#endif // !_SETTINGS_H_
