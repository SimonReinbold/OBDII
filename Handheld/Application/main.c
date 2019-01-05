#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <error_defs.h>

#include "../LCD/lcd-routines.h"
#include "Buttons/buttonHandler.h"
#include "Menu/Menu.h"
#include "ApplicationLayer_KWP2000/include/ApplicationLayer_KWP2000.h"

#include <USART/DataLayer/include/dataLayer_USART.h>

void boot();
void decodeError(unsigned char error);

int main() {
	boot();
	
	unsigned char error;

	while (1) {
		waitForButtonRelease();
		waitForButtonPress();
		waitForButtonRelease();
		switch (button_pressed) {
			case 1:
				if (currentMenuItem->before != NULL) {
					currentMenuItem = currentMenuItem->before;
				}
				break;
			case 2:
				if (currentMenuItem->next != NULL) {
					currentMenuItem = currentMenuItem->next;
				}
				break;
			case 3:
				error = currentMenuItem->execute();
				decodeError(error);
				_delay_ms(1000);
				//if (error == CODE_OK) {
					if (currentMenuItem->submenu) {
						menuLayerDown();
					}
				//}
				/*
				else {
					
				}*/
				break;
		}
		showMenu();
	}
}

void boot() {
	lcd_init();
	init_buttons();
	init_applicationLayer_KWP2000();
	init_dataLayer_USART();
	createMenus();

	show_version();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);

	showMenu();
	sei();
}

void decodeError(unsigned char error) {
	if (error == CODE_OK){
		lcd_setcursor(14, 2);
		lcd_data('O');
		lcd_data('K');
	}
	else {
		lcd_setcursor(10, 2);
		lcd_data('E');
		lcd_data('r');
		lcd_data('r');
		lcd_data(' ');
		HextoASCII(&error);
	}
	return;
	switch (error)
	{
	case CODE_OK:
		lcd_string("OK");
		break;
	case CODE_DATA_ERROR:
		lcd_string("DATA ERROR");
		break;
	case CODE_ERROR:
		lcd_string("ERROR");
		break;
	case CODE_BUS_ERROR_START:
		lcd_string("BUS ERROR START");
		break;
	case CODE_BUS_ERROR_STOP:
		lcd_string("BUS ERROR STOP");
		break;
	case CODE_NO_DATA:
		lcd_string("NO DATA");
		break;
	case CODE_CHECKSUM_ERROR_KWP2000:
		lcd_string("CHECK ERR KWP");
		break;
	case CODE_CHECKSUM_ERROR_USART:
		lcd_string("CHECK ERR USART");
		break;
	case CODE_NEGATIVE_RESPONSE:
		lcd_string("START COM FAILED");
		break;
	case CODE_MANUAL_STOP:
		lcd_string("MANUAL STOP");
		break;
	case CODE_KEYBYTE_ERROR:
		lcd_string("KEYBYTE ERROR");
		break;
	default:
		lcd_display(&error, 1, 2);
		break;
	}
}