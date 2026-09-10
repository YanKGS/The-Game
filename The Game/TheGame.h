#ifndef __THEGAME__
#define __THEGAME__

#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_W 520
#define SCREEN_H 680
#define END 3000

enum GameState { MENU, PLAYING, GAMEOVER, GAMEWIN, PAUSE, SAIR };
enum Hitbox { NO, YES };

typedef struct {
  float x, y;
  float vel_x, vel_y;
  int no_chao;
  int height, width;
  int hb_width, hb_height;
  int hb_x, hb_y;
} Entity;

void draw_game_over(ALLEGRO_FONT* font);
void draw_game_win(ALLEGRO_FONT* font);
#endif