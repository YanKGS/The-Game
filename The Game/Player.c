#include "Player.h"

PlayerSprites* sprites_create(const char* player_jump, const char* player_walk,
                              const char* player_idle,
                              const char* player_crouching,
                              const char* player_escada) {
  PlayerSprites* new_sprites = malloc(sizeof(PlayerSprites));
  if (!new_sprites) {
    return NULL;
  }

  new_sprites->jump = al_load_bitmap(player_jump);
  new_sprites->walk = al_load_bitmap(player_walk);
  new_sprites->idle = al_load_bitmap(player_idle);
  new_sprites->crouching = al_load_bitmap(player_crouching);
  new_sprites->escada = al_load_bitmap(player_escada);
  return new_sprites;
}
Player* player_create(unsigned char side, float x, float y) {
  Player* new_player = malloc(sizeof(Player));
  if (!new_player) {
    return NULL;
  }

  new_player->side = side;
  new_player->x = x;
  new_player->y = y;
  new_player->vel_x = 0;
  new_player->vel_y = 0;
  new_player->no_chao = 1;
  new_player->height = PLAYER_HEIGHT;
  new_player->width = PLAYER_WIDTH;
  new_player->hb_width = PLAYER_WIDTH;
  new_player->hb_height = PLAYER_HEIGHT;
  new_player->hb_x = (PLAYER_WIDTH - new_player->hb_width) / 2;
  new_player->hb_y = (PLAYER_HEIGHT - new_player->hb_height) / 2;
  new_player->lives = 3;
  new_player->invencivel = 0;
  new_player->invencivel_timer = INVENCIVEL_FRAMES;
  new_player->ladder = 0;
  new_player->x_over = x;
  new_player->y_over = y;
  return new_player;
}
void player_status(Player* player) {
  printf("--------------------------------------------\n");
  printf("Player x: %f | Player y: %f\n", player->x, player->y);
  printf("Player x_speed: %f | Player y_speed: %f\n ", player->vel_x,
         player->vel_y);
}

void player_move(Player* player, ALLEGRO_KEYBOARD_STATE* teclado,
                 Platform** platforms, int count) {
  if (!player) return;

  int estava_no_chao = player->no_chao;

  player->vel_x = 0;

  if (al_key_down(teclado, ALLEGRO_KEY_LEFT)) {
    player->vel_x -= PLAYER_STEP;
    player->side = ESQUERDA;
  }
  if (al_key_down(teclado, ALLEGRO_KEY_RIGHT)) {
    player->vel_x += PLAYER_STEP;
    player->side = DIREITA;
  }
  if (al_key_down(teclado, ALLEGRO_KEY_UP) && estava_no_chao) {
    player->vel_y = -PLAYER_JUMP;
    player->no_chao = 0;
  }

  if (al_key_down(teclado, ALLEGRO_KEY_DOWN) && !player->crouching) {
    player->crouching = 1;
    player->hb_height = PLAYER_HEIGHT / 2;
    player->hb_y = PLAYER_HEIGHT / 2;
  }

  // Verifica se pode levantar
  int pode_levantar = 1;
  for (int i = 0; i < count; i++) {
    float topo_player =
        player->y + player->hb_y - (PLAYER_HEIGHT - player->hb_height);
    if (topo_player < platforms[i]->y + platforms[i]->height &&
        player->y + player->hb_y + player->hb_height > platforms[i]->y &&
        player->x + player->hb_x + player->hb_width > platforms[i]->x &&
        player->x + player->hb_x < platforms[i]->x + platforms[i]->width) {
      pode_levantar = 0;
    }
  }

  // Levantar quando soltar S
  if (!al_key_down(teclado, ALLEGRO_KEY_DOWN) && player->crouching) {
    if (pode_levantar) {
      player->crouching = 0;
      player->hb_height = PLAYER_HEIGHT;
      player->hb_y = 0;
    }
  }

  // Detecção de borda (cair no vazio)
  if (player->no_chao && player->vel_y == 0) {
    player->y += 1.0f;
    int ainda_no_chao = 0;
    for (int i = 0; i < count; i++) {
      if (platform_check_collision(platforms[i], (Entity*)player) == TOP) {
        ainda_no_chao = 1;
        break;
      }
    }
    player->y -= 1.0f;
    if (!ainda_no_chao) player->no_chao = 0;
  }

  // Aplica movimento
  player->y += player->vel_y;
  player->x += player->vel_x;

  // Limite esquerdo
  if (player->x + player->hb_x < 0) {
    player->x = -player->hb_x;
    player->vel_x = 0;
  }

  // Colisão com plataformas
  for (int i = 0; i < count; i++) {
    int col = platform_check_collision(platforms[i], (Entity*)player);
    switch (col) {
      case TOP:
        player->vel_y = 0;
        player->no_chao = 1;
        player->y = platforms[i]->y - player->hb_y - player->hb_height;
        break;
      case BOTTOM:
        player->vel_y = 0;
        player->y = platforms[i]->y + platforms[i]->height - player->hb_y;
        break;
      case LEFT:
        player->vel_x = 0;
        player->x = platforms[i]->x - player->hb_x - player->hb_width;
        break;
      case RIGHT:
        player->vel_x = 0;
        player->x = platforms[i]->x + platforms[i]->width - player->hb_x;
        break;
    }
  }

  // Colisão com o chão da tela
  if (player->y + player->hb_y + player->hb_height >= SCREEN_H) {
    player->y = SCREEN_H - player->hb_y - player->hb_height;
    player->vel_y = 0;
    player->no_chao = 1;
  }

  // Gravidade
  if (!player->no_chao) {
    player->vel_y += GRAVIDADE;
  }

  // Escada
  if (player->ladder) {
    player->vel_y = 0;
    player->vel_x = 0;
    if (al_key_down(teclado, ALLEGRO_KEY_UP)) {
      player->vel_y = -PLAYER_STEP;
    }
    if (al_key_down(teclado, ALLEGRO_KEY_DOWN)) {
      player->vel_y = PLAYER_STEP;
    }
  }
}

void hitbox_draw(Player* player) {
  if (!player) {
    return;
  }

  al_draw_rectangle(player->x + player->hb_x, player->y + player->hb_y,
                    player->x + player->hb_x + player->hb_width,
                    player->y + player->hb_y + player->hb_height,
                    al_map_rgb(255, 0, 0), 1.0f);
}
// Desenha o player
// Como varios sprites tem tamanhos diferentes, tem-se de fazer em escala
void player_draw(Player* player, PlayerSprites* sprites) {
  if (player->invencivel && (player->invencivel_timer / 4) % 2 == 0) {
    return;
  }
  int nova_anim =
      player->ladder
          ? 4
          : (player->crouching
                 ? 3
                 : (!player->no_chao ? 2 : (player->vel_x == 0 ? 0 : 1)));

  if (nova_anim != sprites->anim_atual) {
    sprites->frame = 0.f;
    sprites->anim_atual = nova_anim;
  }

  float scale = 1.8f;
  float sprite_w = 128 * scale;
  float sprite_h = 128 * scale;
  float offset_x = (sprite_w - player->width) / 2;
  float offset_y = (sprite_h - player->height) / 2;

  if (player->ladder) {  // Escada
    float scale_l = 0.55f;
    float sw = 100 * scale_l;
    float sh = 500 * scale_l;
    float ox = (sw - player->width) / 2 - 20.f;
    float oy = (sh - player->height) / 2;

    if (player->vel_y != 0) {
      sprites->frame += 0.1f;
      if (sprites->frame >= 5) sprites->frame = 0;
    } else {
      sprites->frame = 0;
    }

    al_draw_scaled_bitmap(
        sprites->escada, 100 * (int)sprites->frame, 0, 100, 500, player->x - ox,
        player->y - oy, sw, sh,
        player->side == DIREITA ? 0 : ALLEGRO_FLIP_HORIZONTAL);

  } else if (player->crouching) {  // crouching

    float scale_s = 0.65f;
    float sw = CROUCHING_FRAME_W * scale_s;
    float sh = CROUCHING_FRAME_H * scale_s;
    float ox = (sw - player->hb_width) / 2 - player->hb_x;
    float oy = (sh - player->height) / 2 - 1.5f;

    if (player->vel_x != 0) {
      sprites->frame += 0.3f;
      if (sprites->frame >= 6) {
        sprites->frame = 0;
      }
    } else {
      sprites->frame = 0;
    }

    al_draw_scaled_bitmap(
        sprites->crouching, CROUCHING_FRAME_W * (int)sprites->frame, 0,
        CROUCHING_FRAME_W, CROUCHING_FRAME_H, player->x - ox, player->y - oy,
        sw, sh, player->side == DIREITA ? 0 : ALLEGRO_FLIP_HORIZONTAL);

  } else if (!player->no_chao && !player->ladder) {
    // jump
    sprites->frame += 0.1f;
    if (sprites->frame >= 6) {
      sprites->frame = 1;
    }

    al_draw_scaled_bitmap(
        sprites->jump, 128 * (int)sprites->frame, 0, 128, 128,
        player->x - offset_x, player->y - offset_y, sprite_w, sprite_h,
        player->side == DIREITA ? 0 : ALLEGRO_FLIP_HORIZONTAL);
  } else if (player->vel_x != 0) {
    // walk
    sprites->frame += 0.5f;
    if (sprites->frame >= 10) {
      sprites->frame = 0;
    }
    al_draw_scaled_bitmap(
        sprites->walk, 128 * (int)sprites->frame, 0, 128, 128,
        player->x - offset_x, player->y - offset_y, sprite_w, sprite_h,
        player->side == DIREITA ? 0 : ALLEGRO_FLIP_HORIZONTAL);
  } else {
    // idle
    sprites->frame += 0.7f;
    if (sprites->frame >= 10) {
      sprites->frame = 0;
    }
    al_draw_scaled_bitmap(sprites->idle, 128 * (int)sprites->frame, 0, 128, 128,
                          player->x - offset_x, player->y - offset_y, sprite_w,
                          sprite_h, 0);
  }
}

// Destrutor do player
void player_destroy(Player* element) {
  if (!element) {
    return;
  }
  free(element);
}

// Reseta as estatisticas do player
void reset_player(Player* player) {
  player->x = player->x_over;
  player->y = player->y_over;
  player->lives = 3;
  player->vel_x = 0;
  player->vel_y = 0;
}

// Destrutor dos sprites
void sprites_destroy(PlayerSprites* sprites) {
  al_destroy_bitmap(sprites->jump);
  al_destroy_bitmap(sprites->walk);
  al_destroy_bitmap(sprites->idle);
  al_destroy_bitmap(sprites->crouching);
  al_destroy_bitmap(sprites->escada);
  free(sprites);
}

// Verifica se o player atingiu o fim
int player_reached_end(Player* player) {
  if (player->x >= END) {
    return 1;
  } else {
    return 0;
  }
}

// Verifica se o player atingiu o chão ou se a vida acabou
int player_is_dead(Player* player) {
  if (player->y >= 616 || player->lives <= 0) {
    return 1;
  } else {
    return 0;
  }
}

// Frames de invencibilidade do player
void player_invencibility(Player* player) {
  if (player->invencivel) {
    player->invencivel_timer--;
    if (player->invencivel_timer <= 0) {
      player->invencivel = 0;
      player->invencivel_timer = 0;
    }
  }
}

float player_get_camera_x(Player* player) { return player->x - SCREEN_W / 2; }

int player_lives(Player* player) { return player->lives; }