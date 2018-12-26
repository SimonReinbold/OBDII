/**************************************************************
*
*
*                  HANDHELD DEVICE
*
*
***************************************************************/

void init_buttons();
unsigned char waitForButtonPress();
unsigned char waitForButtonRelease();

#define BUTTON_REG	DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN	PIND
#define BUTTON_1	PD4
#define BUTTON_2	PD3
#define BUTTON_3	PD2

unsigned char button_pressed;