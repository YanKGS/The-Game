#ifndef __PLAYER__
#define __PLAYER__

#include "Platform.h"
#include "TheGame.h"

#define PLAYER_STEP 5.0f
#define PLAYER_JUMP 12.0f
#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 64
#define GRAVIDADE 0.5f
#define ANIM_SPEED 16
#define INVENCIVEL_FRAMES 90
#define CROUCHING_FRAME_W 93
#define CROUCHING_FRAME_H 447

enum LADOS {
  ESQUERDA = 0,
  DIREITA = 1,
};

typedef struct Player {
  float x, y;
  float vel_x, vel_y;
  int no_chao;
  int height, width;
  int hb_width, hb_height;
  int hb_x, hb_y;
  unsigned char side;
  int lives;
  int invencivel;
  int invencivel_timer;
  int ladder;
  int crouching;
  int agachado_em_movimento;
  float x_over, y_over;
} Player;

typedef struct {
  ALLEGRO_BITMAP* idle;
  ALLEGRO_BITMAP* walk;
  ALLEGRO_BITMAP* jump;
  ALLEGRO_BITMAP* crouching;
  ALLEGRO_BITMAP* escada;
  float frame;
  int current_frame_y;
  int anim_atual;
} PlayerSprites;

Player* player_create(unsigned char side, float x, float y);
PlayerSprites* sprites_create(const char* player_jump, const char* player_walk,
                              const char* player_idle,
                              const char* player_agachado,
                              const char* player_escada);
void player_move(Player* element, ALLEGRO_KEYBOARD_STATE* teclado,
                 Platform** platforms, int count);
void player_draw(Player* player, PlayerSprites* sprites);
void player_destroy(Player* element);
void player_status(Player* player);
int player_reached_end(Player* player);
int player_is_dead(Player* player);
void reset_player(Player* player);
void sprites_destroy(PlayerSprites* sprites);
void player_invencibility(Player* player);
float player_get_camera_x(Player* player);
int player_lives(Player* player);
void hitbox_draw(Player* player);

#endif