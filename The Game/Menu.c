#include "Menu.h"

// Inicializa a estrutura do Menu Principal
Menu* menu_create(ALLEGRO_FONT* font, const char* path) {
  Menu* new_menu = malloc(sizeof(Menu));
  if (!new_menu) {
    return NULL;
  }

  new_menu->selected = 0;
  new_menu->background = al_load_bitmap(path);
  new_menu->font = font;
  return new_menu;
}

// Desenha o fundo esticado, os textos e o retângulo indicador de seleção
void menu_draw(Menu* menu) {
  if (!menu) {
    return;
  }

  // Limpa a tela antes com um tom escuro
  al_clear_to_color(al_map_rgb(12, 17, 34));

  // Renderiza a imagem
  al_draw_scaled_bitmap(
      menu->background, 0, 0, al_get_bitmap_width(menu->background),
      al_get_bitmap_height(menu->background), 0, 0, SCREEN_W, SCREEN_H, 0);

  // Operador ternário
  ALLEGRO_COLOR color_new = (menu->selected == 0) ? al_map_rgb(255, 255, 0)
                                                  : al_map_rgb(255, 255, 255);
  ALLEGRO_COLOR color_esc = (menu->selected == 1) ? al_map_rgb(255, 255, 0)
                                                  : al_map_rgb(255, 255, 255);

  // Desenha os textos das opções centralizados horizontalmente no meio da tela
  al_draw_text(menu->font, color_new, SCREEN_W / 2, 270, ALLEGRO_ALIGN_CENTER,
               "Novo Jogo");
  al_draw_text(menu->font, color_esc, SCREEN_W / 2, 385, ALLEGRO_ALIGN_CENTER,
               "Sair do Jogo");

  // Define os limites laterais da caixinha de seleção amarela
  int box_left = SCREEN_W / 2 - 100;
  int box_right = SCREEN_W / 2 + 100;

  // Renderiza a caixinha em volta do texto correto dependendo do índice
  switch (menu->selected) {
    case 0:  // Novo Jogo
      al_draw_rectangle(box_left, 210, box_right, 340, al_map_rgb(255, 255, 0),
                        3);
      break;
    case 1:  // Sair do Jogo
      al_draw_rectangle(box_left, 320, box_right, 450, al_map_rgb(255, 255, 0),
                        3);
      break;
  }
}

// Capta as entradas do teclado para navegar e acionar as opções do menu
void menu_select(Menu* menu, ALLEGRO_EVENT* event, enum GameState* state) {
  if (!menu || !event || !state) {
    return;
  }
  if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
    // Pressionou seta para baixo: avança no menu
    if (event->keyboard.keycode == ALLEGRO_KEY_DOWN) {
      menu->selected++;
      if (menu->selected > 1)
        menu->selected = 0;  // Efeito loop: passou do limite, volta pro topo
    }

    // Pressionou seta para cima: recua no menu
    if (event->keyboard.keycode == ALLEGRO_KEY_UP) {
      menu->selected--;
      if (menu->selected < 0)
        menu->selected = 1;  // Efeito loop: subiu além do topo, vai pro final
    }

    // Trata o clique do ENTER com base na opção real que está ativa
    if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
      switch (menu->selected) {
        case 0:
          *state =
              PLAYING;  // Muda o estado geral do jogo para iniciar o gameplay
          break;
        case 1:
          *state = SAIR;
          break;
      }
    }
  }
}

void menu_destroy(Menu* menu) {
  if (!menu) {
    return;
  }

  al_destroy_bitmap(menu->background);
  free(menu);
}