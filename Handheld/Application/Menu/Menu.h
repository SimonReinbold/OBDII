#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _MENU_H_
#define _MENU_H_

typedef struct menu_t {
	char name[17];
	struct node_t* first;
	struct menu_t* parent;
}Menu;

typedef struct node_t {
	char name[17];
	unsigned char(*execute)();
	Menu* submenu;
	struct node_t * next;
	struct node_t * before;
}Node;

void showMenu();
void createMenus();
unsigned char show_version();
unsigned char menuLayerUp();
unsigned char menuLayerDown();

Menu* currentMenu;
Node* currentNode;

#endif // ! _MENU_H_