/**************************************************************
*
*
*                  HANDHELD DEVICE
*
*
***************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <USART/PhysicalLayer/include/physicalLayer_USART.h>

#include "buttonHandler.h"

#define DEBOUNCE_DELAY	30

unsigned char checkButtonPressed();
unsigned char checkButtonReleased();
void enableINT0();
void disableINT0();

void init_buttons() {
	BUTTON_REG &= ~(1 << BUTTON_1);
	BUTTON_PORT |= (1 << BUTTON_1);

	BUTTON_REG &= ~(1 << BUTTON_2);
	BUTTON_PORT |= (1 << BUTTON_2);

	BUTTON_REG &= ~(1 << BUTTON_3);
	BUTTON_PORT |= (1 << BUTTON_3);
}

void enableManualStop() {
	enableINT0();
}

void enableINT0() {
	EICRA |= 1 << ISC01;
	EIFR |= 1 << INTF0;
	EIMSK |= 1 << INT0;
}

void disableINT0() {
	EIMSK &= ~(1 << INT0);
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

ISR(INT0_vect) {
	setManualRXTrigger();
	disableINT0();
}