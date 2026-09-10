#ifndef __LADDER__
#define __LADDER__

#include "Player.h"
#include "TheGame.h"
#define SPRITE_HEIGHT 128
#define SPRITE_WIDTH 585

typedef struct Ladder {
  int height, width;
  float x, y;
  ALLEGRO_BITMAP* sprite;
} Ladder;

Ladder* ladder_create(int height, int width, float x, float y,
                      const char* path);
void ladder_draw(Ladder* escada);
void ladder_destroy(Ladder* ladder);
void check_colision_player_ladder(Ladder* escada, Player* player);

#endif