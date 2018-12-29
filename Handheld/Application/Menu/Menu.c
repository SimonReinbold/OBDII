#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include "../../LCD/lcd-routines.h"

#include <stdlib.h>
#include <string.h>
#include "Menu.h"
#include "../../USART/ApplicationLayer/include/applicationLayer_USART.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

const char version[] = STR(GIT_VERSION);

unsigned char doNothing();
unsigned char reboot();

Node* newNode();
Menu* newMenu();

void showMenu() {
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string(currentMenu->name);
	lcd_setcursor(0, 2);
	lcd_string(currentNode->name);
}

Node* newNode() {
	Node* dummy = malloc(sizeof(Node));
	dummy->before = NULL;
	dummy->next = NULL;
	strcpy(dummy->name, "Unknown");
	dummy->execute = NULL;
	dummy->submenu = 0;
	dummy->execute = doNothing;
	return dummy;
}

Menu* newMenu() {
	Menu* dummy = malloc(sizeof(Menu));
	strcpy(dummy->name, "Unknown");
	dummy->first = NULL;
	dummy->parent = NULL;
	return dummy;
}

void createMenus() {
	Node* back = newNode();
	strcpy(back->name, "Back");
	back->execute = menuLayerUp;

	//Top Level
	Menu* topLvl = newMenu();
	strcpy(topLvl->name, "-----OBDII------");

	Node* restart = newNode();
	strcpy(restart->name, "Restart");
	restart->execute = reboot;
	Node* init = newNode();
	strcpy(init->name, "Init Diag");
	init->execute = start_communication_fastInit;
	Node* showVersion = newNode();
	strcpy(showVersion->name, "Show Version");
	showVersion->execute = show_version;

	restart->next = init;
	init->next = showVersion;

	init->before = restart;
	showVersion->before = init;

	topLvl->first = restart;

	// Diagnose Level
	Menu* diagLvl = newMenu();
	init->submenu = diagLvl;
	diagLvl->parent = topLvl;
	strcpy(diagLvl->name, "----Diagnose----");

	Node* checkPID = newNode();
	strcpy(checkPID->name, "Check PID");
	checkPID->execute = requestPIDs;
	Node* stopCom = newNode();
	strcpy(stopCom->name, "Stop COM");
	stopCom->execute = stop_communication;

	checkPID->next = stopCom;
	stopCom->next = back;

	stopCom->before = checkPID;
	back->before = stopCom;

	diagLvl->first = checkPID;

	currentMenu = topLvl;
	currentNode = currentMenu->first;
}

unsigned char menuLayerUp() {
	if (currentMenu->parent != NULL) {
		currentMenu = currentMenu->parent;
		currentNode = currentMenu->first;
	}
}

unsigned char menuLayerDown() {
	if (currentNode->submenu != NULL) {
		currentMenu = currentNode->submenu;
		currentNode = currentMenu->first;
	}

}

unsigned char doNothing() {
	//
}

unsigned char reboot() {
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR |= (1 << WDE);
	while (1);
}

unsigned char show_version() {
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string("OBD2 Diagnose");

	lcd_setcursor(0, 2);
	for (int i = 0; i < VERS_CHARS; i++) {
		lcd_data(version[i]);
	}

	_delay_ms(2000);
	lcd_clear();
}