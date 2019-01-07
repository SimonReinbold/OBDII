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
	
	MenuItem* showDTCs = newMenuItem();
	strcpy(showDTCs->name, "Show DTCs");
	showDTCs->execute = requestDTCs;

	MenuItem* engineLoad = newMenuItem();
	strcpy(engineLoad->name, "Engine Load");
	engineLoad->execute = calcEngineLoad;
	MenuItem* coolantTemp = newMenuItem();
	strcpy(coolantTemp->name, "Coolant Temp");
	coolantTemp->execute = engineCoolantTemp;
	MenuItem* intakePressure = newMenuItem();
	strcpy(intakePressure->name, "Intake Abs Press");
	intakePressure->execute = intakeManifoldAbsolutePressure;
	MenuItem* engineRPMItem = newMenuItem();
	strcpy(engineRPMItem->name, "Engine RPM");
	engineRPMItem->execute = engineRPM;
	MenuItem* vehicleSpeedItem = newMenuItem();
	strcpy(vehicleSpeedItem->name, "Vehicle Speed");
	vehicleSpeedItem->execute = vehicleSpeed;
	MenuItem* intakeAirTemp = newMenuItem();
	strcpy(intakeAirTemp->name, "Intake Air Temp");
	intakeAirTemp->execute = intake_air_Temp;
	MenuItem* airFlowRate = newMenuItem();
	strcpy(airFlowRate->name, "Air Flow Rate");
	airFlowRate->execute = mafAirFlowRate;
	MenuItem* throttlePos = newMenuItem();
	strcpy(throttlePos->name, "Throttle Pos.");
	throttlePos->execute = throttlePosition;

	MenuItem* stopCom = newMenuItem();
	strcpy(stopCom->name, "Stop COM");
	stopCom->execute = stop_communication;

	showDTCs->next = engineLoad;
	engineLoad->next = coolantTemp;
	coolantTemp->next = intakePressure;
	intakePressure->next = engineRPMItem;
	engineRPMItem->next = vehicleSpeedItem;
	vehicleSpeedItem->next = intakeAirTemp;
	intakeAirTemp->next = airFlowRate;
	airFlowRate->next = throttlePos;
	throttlePos->next = stopCom;
	stopCom->next = back;

	back->before = stopCom;
	stopCom->before = throttlePos;
	throttlePos->before = airFlowRate;
	airFlowRate->before = intakeAirTemp;
	intakeAirTemp->before = vehicleSpeedItem;
	vehicleSpeedItem->before = engineRPMItem;
	engineRPMItem->before = intakePressure;
	intakePressure->before = coolantTemp;
	coolantTemp->before = engineLoad;
	engineLoad->before = showDTCs;

	diagLvl->first = showDTCs;

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