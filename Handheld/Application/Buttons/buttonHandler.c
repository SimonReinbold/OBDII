/**************************************************************
*
*
*                  HANDHELD DEVICE
*
*
***************************************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "buttonHandler.h"

#define DEBOUNCE_DELAY	30

unsigned char checkButtonPressed();
unsigned char checkButtonReleased();

void init_buttons() {
	BUTTON_REG &= ~(1 << BUTTON_1);
	BUTTON_PORT |= (1 << BUTTON_1);

	BUTTON_REG &= ~(1 << BUTTON_2);
	BUTTON_PORT |= (1 << BUTTON_2);

	BUTTON_REG &= ~(1 << BUTTON_3);
	BUTTON_PORT |= (1 << BUTTON_3);
}

unsigned char checkButtonPressed() {
	if (!(BUTTON_PIN & (1 << BUTTON_1))) {
		button_pressed = 1;
		return 1;
	}
	if (!(BUTTON_PIN & (1 << BUTTON_2))) {
		button_pressed = 2;
		return 2;
	}
	if (!(BUTTON_PIN & (1 << BUTTON_3))) {
		button_pressed = 3;
		return 3;
	}
	return 0;
}

unsigned char waitForButtonPress() {
	while (!checkButtonPressed());
	_delay_ms(DEBOUNCE_DELAY);
	while (!checkButtonPressed());
	return button_pressed;
}

unsigned char checkButtonReleased() {
	if ((BUTTON_PIN & (1 << BUTTON_1)) && (BUTTON_PIN & (1 << BUTTON_2)) && (BUTTON_PIN & (1 << BUTTON_3))) {
		return 1;
	}
	return 0;
}

unsigned char waitForButtonRelease() {
	while (!checkButtonReleased());
	_delay_ms(DEBOUNCE_DELAY);
	while (!checkButtonReleased());
}