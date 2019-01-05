#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <error_defs.h>
#include "../../LCD/lcd-routines.h"

#include <stdlib.h>
#include <string.h>
#include "Menu.h"
#include "../ApplicationLayer_KWP2000/include/ApplicationLayer_KWP2000.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

const char version[] = STR(GIT_VERSION);

unsigned char doNothing();
unsigned char reboot();

MenuItem* newMenuItem();
Menu* newMenu();

void showMenu() {
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string(currentMenu->name);
	lcd_setcursor(0, 2);
	lcd_string(currentMenuItem->name);
}

MenuItem* newMenuItem() {
	MenuItem* dummy = malloc(sizeof(MenuItem));
	dummy->before = NULL;
	dummy->next = NULL;
	strcpy(dummy->name, "Unknown");
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
	MenuItem* back = newMenuItem();
	strcpy(back->name, "Back");
	back->execute = menuLayerUp;

	//Top Level
	Menu* topLvl = newMenu();
	strcpy(topLvl->name, "-----OBDII------");

	MenuItem* restart = newMenuItem();
	strcpy(restart->name, "Restart");
	restart->execute = reboot;
	MenuItem* init = newMenuItem();
	strcpy(init->name, "Init Diag");
	init->execute = start_communication_fastInit;
	MenuItem* showVersion = newMenuItem();
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

	MenuItem* checkPID = newMenuItem();
	strcpy(checkPID->name, "Check PID");
	checkPID->execute = requestPIDs;
	MenuItem* stopCom = newMenuItem();
	strcpy(stopCom->name, "Stop COM");
	stopCom->execute = stop_communication;
	MenuItem* intakeAirTemp = newMenuItem();
	strcpy(intakeAirTemp->name, "Intake Air Temp");
	intakeAirTemp->execute = intake_air_Temp;

	checkPID->next = stopCom;
	stopCom->next = intakeAirTemp;
	intakeAirTemp->next = back;

	stopCom->before = checkPID;
	intakeAirTemp->before = stopCom;
	back->before = intakeAirTemp;

	diagLvl->first = checkPID;

	currentMenu = topLvl;
	currentMenuItem = currentMenu->first;
}

unsigned char menuLayerUp() {
	if (currentMenu->parent != NULL) {
		currentMenu = currentMenu->parent;
		currentMenuItem = currentMenu->first;
	}
	return CODE_OK;
}

unsigned char menuLayerDown() {
	if (currentMenuItem->submenu != NULL) {
		currentMenu = currentMenuItem->submenu;
		currentMenuItem = currentMenu->first;
	}
	return CODE_OK;
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