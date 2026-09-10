#ifndef __BACKGROUND__
#define __BACKGROUND__

#include "TheGame.h"

#define BACKGROUND_WIDTH 5000
#define BACKGROUND_HEIGHT 610

typedef struct {
  ALLEGRO_BITMAP* back;
} BackgroundSprites;

BackgroundSprites* background_create(const char* back_path);
void background_draw(BackgroundSprites* sprite);
void background_resize(BackgroundSprites* sprite);
void background_destroy(BackgroundSprites* bg);

#endif