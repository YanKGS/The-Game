#ifndef __ENEMY__
#define __ENEMY__

#include "Platform.h"
#include "TheGame.h"

#define ENEMY_RANGE 100.0f
#define ENEMY_STEP 2.f
#define GRAVIDADE 0.5f
#define ENEMIES 9

typedef struct Enemy {
  float x, y;
  float vel_x, vel_y;
  int no_chao;
  int height, width;
  int hb_width, hb_height;
  int hb_x, hb_y;
  float spawn_x, spawn_y;
  int movel;
  int movimento;
  ALLEGRO_BITMAP* sprite;
  float frame;
  int frame_count;
  int frame_w, frame_h;
  int ativo;
  int laser_timer;
  int laser_ligado;
} Enemy;

Enemy* enemy_create(float x, float y, int movel, int movimento, int no_chao,
                    int height, int width, float spawn_x, float spawn_y,
                    const char* sprite_path, int frame_count, int frame_w,
                    int frame_h, int hb_width, int hb_height, int ativo);
void enemy_movimentation(Enemy* inimigo, Platform** platforms, int count);
void enemy_draw(Enemy* inimigo);
void enemy_destroy(Enemy* inimigo);
void enemy_check_player_collision(Enemy* inimigo, Player* player);
int enemy_is_movable(Enemy* enemy);
void enemy_activate_on_trigger(Enemy* inimigo, Player* player, float trigger_x);
void hitbox_draw_e(Enemy* inimigo);

#endif