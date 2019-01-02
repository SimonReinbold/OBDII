#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <error_defs.h>

#include "../LCD/lcd-routines.h"
#include "Buttons/buttonHandler.h"
#include "Menu/Menu.h"
#include "ApplicationLayer_KWP2000/include/ApplicationLayer_KWP2000.h"
#include "../USART/DataLayer/include/dataLayer_USART.h"

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
			if (currentNode->before != NULL) {
				currentNode = currentNode->before;
			}
			break;
		case 2:
			if (currentNode->next != NULL) {
				currentNode = currentNode->next;
			}
			break;
		case 3:
			error = currentNode->execute();
			lcd_clear();
			decodeError(error);
			lcd_display(&msg.data[3],msg.length-4,2);
			_delay_ms(1000);
			if (error == CODE_OK) {
				if (currentNode->submenu) {
					menuLayerDown();
				}
			}
			break;
		}
		showMenu();
	}
}

void boot() {
	lcd_init();
	init_buttons();
	init_applicationLayer_KWP2000();
	createMenus();

	show_version();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);

	showMenu();
}

void decodeError(unsigned char error) {
	lcd_setcursor(0, 1);
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
		lcd_string("CHECKSUM ERROR KWP2000");
		break;
	case CODE_CHECKSUM_ERROR_USART:
		lcd_string("CHECKSUM ERROR USART");
		break;
	case CODE_NEGATIVE_RESPONSE:
		lcd_string("START COM FAILED");
		break;
	default:
		lcd_display(&error, 1, 2);
		break;
	}
}