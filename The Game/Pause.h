#ifndef __PAUSE__
#define __PAUSE__

#include "TheGame.h"

typedef struct {
  int selected;  // 0 = Continuar, 1 = Menu Principal
} Pause;

Pause* pause_create();
void pause_draw(Pause* pause, ALLEGRO_FONT* font);
void pause_select(Pause* pause, ALLEGRO_EVENT* event, enum GameState* state,
                  enum Hitbox* hit);
void pause_destroy(Pause* pause);

#endif