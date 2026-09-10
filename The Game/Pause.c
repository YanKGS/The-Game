#include "Pause.h"

// Inicializa o estado de controle interno
Pause* pause_create() {
  Pause* new_pause = malloc(sizeof(Pause));
  if (!new_pause) return NULL;
  new_pause->selected = 0;
  return new_pause;
}

// Desenha a interface de pausa aplicando um overlay escurecido
void pause_draw(Pause* pause, ALLEGRO_FONT* font) {
  if (!pause || !font) {
    return;
  }

  al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba(0, 0, 0, 150));

  al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 - 40,
               ALLEGRO_ALIGN_CENTER, "PAUSADO");

  al_draw_text(font,
               pause->selected == 0 ? al_map_rgb(255, 255, 0)
                                    : al_map_rgb(255, 255, 255),
               SCREEN_W / 2, SCREEN_H / 2 + 20, ALLEGRO_ALIGN_CENTER,
               "Continuar");

  al_draw_text(font,
               pause->selected == 1 ? al_map_rgb(255, 255, 0)
                                    : al_map_rgb(255, 255, 255),
               SCREEN_W / 2, SCREEN_H / 2 + 60, ALLEGRO_ALIGN_CENTER,
               "Menu Principal");
  al_draw_text(font,
               pause->selected == 2 ? al_map_rgb(255, 255, 0)
                                    : al_map_rgb(255, 255, 255),
               SCREEN_W / 2, SCREEN_H / 2 + 100, ALLEGRO_ALIGN_CENTER,
               "Habilitar Hitbox");
}

// Controla a navegação via teclado dentro da janela suspensa de pausa
void pause_select(Pause* pause, ALLEGRO_EVENT* event, enum GameState* state,
                  enum Hitbox* hit) {
  if (!pause || !event || !state) {
    return;
  }
  if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
    if (event->keyboard.keycode == ALLEGRO_KEY_DOWN) {
      pause->selected++;
      if (pause->selected > 2) {
        pause->selected = 0;
      }
    }
    if (event->keyboard.keycode == ALLEGRO_KEY_UP) {
      pause->selected--;
      if (pause->selected < 0) {
        pause->selected = 2;
      }
    }

    // Confirmação com a tecla ENTER
    if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
      if (pause->selected == 0) {
        *state = PLAYING;  // Despausa o jogo e retorna à ação normal
      } else if (pause->selected == 1) {
        *state = MENU;  // Abandona a partida em andamento e força o retorno ao
                        // menu principal
      } else if (pause->selected == 2) {
        if (*hit == YES) {
          *hit = NO;
        } else {
          *hit = YES;
        }
      }
    }
  }
}

void pause_destroy(Pause* pause) {
  if (!pause) {
    return;
  }
  free(pause);
}