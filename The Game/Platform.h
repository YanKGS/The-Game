#ifndef __PLATFORM__
#define __PLATFORM__

#include "TheGame.h"

#define PLATFORMS 11
#define PLAT_WIDTH 192
#define PLAT_HEIGHT 64
#define PLATAFORMA_TIMER 60   // frames até sumir
#define PLATAFORMA_RESET 120  // frames até reaparecer

typedef struct Player Player;
typedef struct Enemy Enemy;

typedef struct Platform {
  float x, y;
  float width, height;
  int colisao, fixo;
  int desaparece;  // 1 se é plataforma que some
  int visivel;     // 1 se está visível
  int ativada;
  float timer;  // conta o tempo após pisar
} Platform;

enum CollisionSide {
  NO_COLLISION = 0,
  TOP = 1,     // caindo, pousou na plataforma
  BOTTOM = 2,  // subindo, bateu a cabeça
  LEFT = 3,    // colidiu pela esquerda
  RIGHT = 4    // colidiu pela direita
};

Platform* platform_create(float x, float y, float width, float height,
                          int desaparece);
void platforms_draw(Platform* plat);
void platform_destroy(Platform* platforms);
int platform_check_collision(Platform* platform, Entity* entidade);
int platform_check_collision_top(Platform* platform, Entity* entidade);
void platform_update(Platform* platform, Entity* entidade);

#endif