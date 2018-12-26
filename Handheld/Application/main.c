#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <error_defs.h>

#include "../LCD/lcd-routines.h"
#include "Buttons/buttonHandler.h"
#include "Menu/Menu.h"
#include "Protocol/ApplicationLayer/include/applicationLayer.h"

void boot();

int main() {
	boot();

	unsigned char error;

	while (1) {
		waitForButtonRelease();
		waitForButtonPress();
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
			if (error == CODE_OK) {
				if (currentNode->submenu) {
					menuLayerDown();
				}
			}
			break;
		}
		waitForButtonRelease();
		showMenu();
	}
}

void boot() {
	lcd_init();
	init_buttons();
	init_applicationLayer();
	createMenus();

	show_version();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);

	showMenu();
}