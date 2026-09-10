#include "Ladder.h"

// Cria o objeto de escada e carrega a folha de sprite/textura correspondente
Ladder* ladder_create(int height, int width, float x, float y,
                      const char* path) {
  Ladder* new_ladder = malloc(sizeof(Ladder));
  if (!new_ladder) {
    return NULL;
  }

  new_ladder->height = height;
  new_ladder->width = width;
  new_ladder->x = x;
  new_ladder->y = y;
  new_ladder->sprite = al_load_bitmap(path);

  return new_ladder;
}

// Desenha a escada escalonando a textura conforme o tamanho real
void ladder_draw(Ladder* escada) {
  if (!escada) {
    return;
  }

  al_draw_scaled_bitmap(escada->sprite, 0, 0,
                        al_get_bitmap_width(escada->sprite),
                        al_get_bitmap_height(escada->sprite), escada->x,
                        escada->y, escada->width, escada->height, 0);
}

// Destrutor padrão
void ladder_destroy(Ladder* escada) {
  if (escada) {
    al_destroy_bitmap(escada->sprite);
    free(escada);
  }
}

// Verifica se a Hitbox do jogador entrou na zona física ocupada pela escada
void check_colision_player_ladder(Ladder* escada, Player* player) {
  if (!escada || !player) {
    return;
  }

  // Teste clássico
  if (player->x + player->hb_x + player->hb_width > escada->x &&
      player->x + player->hb_x < escada->x + escada->width &&
      player->y + player->hb_y + player->hb_height > escada->y &&
      player->y + player->hb_y < escada->y + escada->height) {
    player->ladder =
        1;  // Jogador está encostando na escada, ativa estado de subir/descer
  } else {
    player->ladder =
        0;  // Fora da escada, a física de gravidade normal volta a agir
  }
}