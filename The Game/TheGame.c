#include "TheGame.h"

#include "Background.h"
#include "Enemy.h"
#include "Ladder.h"
#include "Menu.h"
#include "Pause.h"
#include "Platform.h"
#include "Player.h"

int main() {
  // Inicialização da biblioteca Allegro e seus addons
  al_init();
  al_init_primitives_addon();
  al_install_keyboard();  // Habilita entrada via teclado
  al_init_image_addon();  // Habilita carregamento de imagens
  al_init_font_addon();   // Habilita o addon de fontes
  al_init_ttf_addon();    // Habilita fontes TTF

  // Timer: controla quantas atualizações por segundo (30 FPS)
  ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);

  // Fila de eventos: captura teclado, timer e eventos da janela
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

  // Fonte padrão bitmap para textos simples
  ALLEGRO_FONT* font = al_create_builtin_font();

  // Janela do jogo com as dimensões definidas em TheGame.h
  ALLEGRO_DISPLAY* disp = al_create_display(SCREEN_W, SCREEN_H);

  // Registra as fontes de eventos na fila
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  // Criação do player e seus sprites
  Player* player = player_create(DIREITA, 0, 540);
  PlayerSprites* sprites = sprites_create(
      "./Sprites/jump.png", "./Sprites/walk.png", "./Sprites/idle.png",
      "./Sprites/walk_s.png", "./Sprites/ladder_removido.png");

  // Criação do background e da escada
  BackgroundSprites* background =
      background_create("./Background/Background.png");
  Ladder* escada =
      ladder_create(250, 30, 90, 340, "./Background/Props/ladder.png");

  // Transformação de câmera: translada o mundo conforme o player se move
  ALLEGRO_TRANSFORM camera;
  float camera_x =
      0;  // Inicializado em 0 para evitar flickering no primeiro frame

  // Criação das plataformas
  Platform* platforms[PLATFORMS];
  platforms[0] = platform_create(0, 600, 550, 200, 0);  // Chão inicial
  platforms[1] =
      platform_create(120, 340, 100, 500, 0);  // Plataforma da escada
  platforms[2] = platform_create(1360, 360, 800, 500, 0);  // Chão do meio
  platforms[3] = platform_create(1010, 480, 100, 20, 0);   // Reta curta
  platforms[4] = platform_create(1110, 420, 150, 20, 0);   // Reta curta
  platforms[5] = platform_create(1400, 0, 100, 315, 0);    // Parede vertical
  platforms[6] =
      platform_create(2300, 500, 100, 20, 1);  // Plataforma que desaparece
  platforms[7] =
      platform_create(2600, 500, 100, 20, 1);  // Plataforma que desaparece
  platforms[8] = platform_create(2800, 500, 500, 500, 0);  // Chão final
  platforms[9] = platform_create(1800, 150, 300, 50, 0);   // Plataforma elevada
  platforms[10] = platform_create(700, 600, 300, 200, 0);  // Chão extra

  // Criação do menu e do pause
  Menu* menu = menu_create(font, "./Background/Menu_background.png");
  Pause* pause = pause_create();

  // Criação dos inimigos
  Enemy* enemies[ENEMIES];
  enemies[0] = enemy_create(300, 536, 1, 0, 0, 64, 80, 300, 400,
                            "./Sprites/Mushroom-Run.png", 8, 80, 64, 35, 40,
                            1);  // Mushroom móvel horizontal
  enemies[1] = enemy_create(500, 400, 1, 1, 0, 64, 64, 500, 400,
                            "./Sprites/Inimigo_voador_move.png", 12, 64, 64, 35,
                            35, 1);  // Voador móvel vertical
  enemies[2] = enemy_create(1600, 360 - 34, 0, 0, 1, 34, 114, 1400, 360 - 34,
                            "./Sprites/espinhos.png", 1, 57, 17, 110, 20,
                            1);  // Espinhos estáticos
  enemies[3] = enemy_create(2750, 400, 1, 1, 0, 64, 64, 500, 400,
                            "./Sprites/Inimigo_voador_move.png", 12, 64, 64, 35,
                            35, 1);  // Voador área final
  enemies[4] = enemy_create(2450, 400, 1, 1, 0, 64, 64, 500, 400,
                            "./Sprites/Inimigo_voador_move.png", 12, 64, 64, 35,
                            35, 1);  // Voador área final
  enemies[5] = enemy_create(1950, 180, 1, 0, 0, 64, 80, 1950, 180,
                            "./Sprites/Mushroom-Run.png", 8, 80, 64, 35, 40,
                            1);  // Mushroom plataforma elevada
  enemies[6] = enemy_create(550, 600, 0, 0, 1, 200, 150, 0, 0,
                            "./Background/Props/lava_river.png", 1, 57, 17, 150,
                            200, 1);  // Rio de lava
  enemies[7] = enemy_create(3000, 600, 0, 0, 1, 50, 100, 0, 0,
                            "./Sprites/log.png", 5, 100, 114, 100, 50,
                            0);  // Tronco animado (ativado por trigger)
  enemies[8] =
      enemy_create(1500, 300, 1, 2, 1, 8, 150, 0, 0, "./Sprites/log.png", 1, 1,
                   1, 150, 8, 1);  // Barreira horizontal invisível

  // Estado inicial do jogo
  enum GameState state = MENU;

  ALLEGRO_KEYBOARD_STATE teclado;
  ALLEGRO_EVENT event;

  // Fonte TTF para o HUD (HP do player)
  ALLEGRO_FONT* font_hud = al_load_ttf_font("./Assets/HUD.ttf", 24, 0);

  // Controla se as hitboxes estão visíveis (ativado pelo menu de pause)
  enum Hitbox hit = NO;

  al_start_timer(timer);

  while (1) {
    al_wait_for_event(queue, &event);

    // Fecha o jogo ao clicar no X da janela
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;

    // Fecha o jogo se o menu pediu para sair
    if (state == SAIR) break;

    al_clear_to_color(al_map_rgb(255, 255, 255));

    // ── MENU ──────────────────────────────────────────────────────────────
    if (state == MENU) {
      menu_draw(menu);
      reset_player(player);  // Reseta o player ao voltar para o menu
      if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        menu_select(menu, &event, &state);
      }
    }

    // ── GAME WIN ──────────────────────────────────────────────────────────
    if (state == GAMEWIN) {
      draw_game_win(font);
      al_get_keyboard_state(&teclado);
      if (al_key_down(&teclado, ALLEGRO_KEY_ESCAPE)) {
        reset_player(player);
        state = MENU;
      }
    }

    // ── GAME OVER ─────────────────────────────────────────────────────────
    if (state == GAMEOVER) {
      draw_game_over(font);
      al_get_keyboard_state(&teclado);
      if (al_key_down(&teclado, ALLEGRO_KEY_ESCAPE)) {
        reset_player(player);
        state = MENU;
      }
    }

    // ── PAUSE ─────────────────────────────────────────────────────────────
    if (state == PAUSE) {
      al_get_keyboard_state(&teclado);

      // Aplica a câmera antes de desenhar o jogo por baixo do pause
      // Sem isso, em eventos de teclado o jogo apareceria sem translação
      al_identity_transform(&camera);
      al_translate_transform(&camera, -camera_x, 0);
      al_use_transform(&camera);

      // Desenha o estado atual do jogo por baixo do overlay
      background_resize(background);
      for (int i = 0; i < PLATFORMS; i++) platforms_draw(platforms[i]);
      for (int i = 0; i < ENEMIES; i++) enemy_draw(enemies[i]);
      ladder_draw(escada);
      player_draw(player, sprites);

      // Remove a câmera para desenhar o overlay em coordenadas de tela
      ALLEGRO_TRANSFORM identidade;
      al_identity_transform(&identidade);
      al_use_transform(&identidade);
      pause_draw(pause, font);

      // Restaura a câmera após o overlay
      al_identity_transform(&camera);
      al_translate_transform(&camera, -camera_x, 0);
      al_use_transform(&camera);

      // Navegação do pause só em eventos de tecla (evita movimentação rápida)
      if (event.type == ALLEGRO_EVENT_KEY_DOWN ||
          event.type == ALLEGRO_EVENT_KEY_UP) {
        pause_select(pause, &event, &state, &hit);
      }
    }

    // ── PLAYING ───────────────────────────────────────────────────────────
    if (state == PLAYING) {
      background_resize(background);

      for (int i = 0; i < PLATFORMS; i++) {
        platform_update(
            platforms[i],
            (Entity*)player);  // Atualiza plataformas que desaparecem
        platforms_draw(platforms[i]);
      }

      al_get_keyboard_state(&teclado);
      ladder_draw(escada);

      for (int i = 0; i < ENEMIES; i++) enemy_draw(enemies[i]);
      player_draw(player, sprites);

      // HUD: exibe o HP do player no canto superior esquerdo da tela
      char hp_text[16];
      sprintf(hp_text, "HP: %d", player_lives(player));
      al_draw_text(font_hud, al_map_rgb(255, 0, 0), camera_x + 10, 10, 0,
                   hp_text);
    }

    // ── LÓGICA DO TIMER (30x por segundo) ─────────────────────────────────
    if (event.type == ALLEGRO_EVENT_TIMER) {
      // ESC durante o jogo entra no pause
      if (al_key_down(&teclado, ALLEGRO_KEY_ESCAPE)) {
        if (state == PLAYING) state = PAUSE;
      }

      if (state == PLAYING) {
        // Ativa o tronco animado quando o player chega perto do final
        enemy_activate_on_trigger(enemies[7], player, 2800);

        // Verifica colisão de todos os inimigos com o player
        for (int i = 0; i < ENEMIES; i++)
          enemy_check_player_collision(enemies[i], player);

        // Desenha hitboxes se habilitado no menu de pause
        if (hit == YES) {
          for (int i = 0; i < ENEMIES; i++) hitbox_draw_e(enemies[i]);
          hitbox_draw(player);
        }

        player_move(player, &teclado, platforms, PLATFORMS);

        if (player_reached_end(player)) state = GAMEWIN;

        // Movimenta os inimigos móveis
        for (int i = 0; i < ENEMIES; i++)
          if (enemy_is_movable(enemies[i]))
            enemy_movimentation(enemies[i], platforms, PLATFORMS);

        if (player_is_dead(player)) state = GAMEOVER;

        check_colision_player_ladder(escada, player);
      }

      // Atualiza a câmera para seguir o player, limitando nas bordas do mapa
      camera_x = player_get_camera_x(player);
      if (camera_x < 0) camera_x = 0;
      if (camera_x > END - SCREEN_W) camera_x = END - SCREEN_W;

      al_identity_transform(&camera);
      al_translate_transform(&camera, -camera_x, 0);
      al_use_transform(&camera);
    }

    al_flip_display();  // Apresenta o frame desenhado na tela
  }

  // ── LIBERAÇÃO DE MEMÓRIA ──────────────────────────────────────────────
  al_destroy_font(font);
  al_destroy_display(disp);
  al_destroy_timer(timer);
  al_destroy_event_queue(queue);
  al_destroy_font(font_hud);
  player_destroy(player);
  sprites_destroy(sprites);
  background_destroy(background);
  ladder_destroy(escada);
  menu_destroy(menu);
  pause_destroy(pause);
  for (int i = 0; i < ENEMIES; i++) enemy_destroy(enemies[i]);
  for (int i = 0; i < PLATFORMS; i++) platform_destroy(platforms[i]);

  return 0;
}

void draw_game_over(ALLEGRO_FONT* font) {
  ALLEGRO_TRANSFORM identidade;
  al_identity_transform(&identidade);
  al_use_transform(&identidade);
  al_clear_to_color(al_map_rgb(0, 0, 0));
  al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2 - 30,
               ALLEGRO_ALIGN_CENTER, "GAME OVER");
  al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2 + 20,
               ALLEGRO_ALIGN_CENTER, "PRESSIONE ESC PARA IR AO MENU");
}

void draw_game_win(ALLEGRO_FONT* font) {
  ALLEGRO_TRANSFORM identidade;
  al_identity_transform(&identidade);
  al_use_transform(&identidade);
  al_clear_to_color(al_map_rgb(0, 0, 0));
  al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2 - 30,
               ALLEGRO_ALIGN_CENTER, "CONGRATULATIONS YOU WIN");
  al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2 + 20,
               ALLEGRO_ALIGN_CENTER, "PRESSIONE ESC PARA IR AO MENU");
}