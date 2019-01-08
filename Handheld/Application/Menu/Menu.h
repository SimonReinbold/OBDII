#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _MENU_H_
#define _MENU_H_

typedef struct menu_t {
	char name[17];
	struct menuItem_t* first;
	struct menu_t* parent;
}Menu;

typedef struct menuItem_t {
	char name[17];
	unsigned char(*execute)();
	Menu* submenu;
	struct menuItem_t * next;
	struct menuItem_t * before;
}MenuItem;

void showMenu();
void createMenus();
unsigned char show_version();
unsigned char menuLayerUp();
unsigned char menuLayerDown();

Menu* currentMenu;
MenuItem* currentMenuItem;

#endif // ! _MENU_H_