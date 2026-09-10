#ifndef __MENU__
#define __MENU__

#include "TheGame.h"

typedef struct {
  int selected;
  ALLEGRO_BITMAP* background;
  ALLEGRO_FONT* font;
} Menu;

Menu* menu_create(ALLEGRO_FONT* font, const char* path);
void menu_draw(Menu* background);
void menu_select(Menu* menu, ALLEGRO_EVENT* event, enum GameState* state);
void menu_destroy(Menu* menu);

#endif