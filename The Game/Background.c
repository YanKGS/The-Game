#include "Background.h"

// Aloca memória e carrega a imagem de fundo do cenário
BackgroundSprites* background_create(const char* back_path) {
  BackgroundSprites* background = malloc(sizeof(BackgroundSprites));

  background->back = al_load_bitmap(back_path);
  return background;
}

void background_resize(BackgroundSprites* sprite) {
  if (!sprite) {
    return;
  }
  al_draw_scaled_bitmap(sprite->back, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT,
                        0, 0, BACKGROUND_WIDTH, SCREEN_H + 10, 0);
}

// Limpeza de memória
void background_destroy(BackgroundSprites* bg) {
  if (bg) {
    al_destroy_bitmap(bg->back);
    free(bg);
  }
}