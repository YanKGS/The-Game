#include "Enemy.h"

#include "Player.h"

// Cria e inicializa um novo inimigo com todos os parâmetros necessários
Enemy* enemy_create(float x, float y, int movel, int movimento, int no_chao,
                    int height, int width, float spawn_x, float spawn_y,
                    const char* sprite_path, int frame_count, int frame_w,
                    int frame_h, int hb_width, int hb_height, int ativo) {
  Enemy* new_enemy = malloc(sizeof(Enemy));  // Aloca memória para o inimigo
  if (!new_enemy) return NULL;  // Retorna NULL se a alocação falhar

  new_enemy->x = x;                  // Posição inicial X
  new_enemy->y = y;                  // Posição inicial Y
  new_enemy->vel_x = 0;              // Velocidade horizontal começa zerada
  new_enemy->vel_y = 0;              // Velocidade vertical começa zerada
  new_enemy->no_chao = no_chao;      // 1 = começa no chão, 0 = no ar
  new_enemy->height = height;        // Altura visual do inimigo em pixels
  new_enemy->width = width;          // Largura visual do inimigo em pixels
  new_enemy->movel = movel;          // 1 = inimigo se move, 0 = estático
  new_enemy->movimento = movimento;  // 0 = horizontal, 1 = vertical, 2 = laser
  new_enemy->spawn_x = spawn_x;      // Posição X de origem (centro da patrulha)
  new_enemy->spawn_y = spawn_y;      // Posição Y de origem (centro da patrulha)
  new_enemy->sprite =
      al_load_bitmap(sprite_path);       // Carrega a imagem do inimigo
  new_enemy->frame = 0;                  // Começa no primeiro frame da animação
  new_enemy->frame_count = frame_count;  // Total de frames na spritesheet
  new_enemy->frame_w = frame_w;          // Largura de cada frame na spritesheet
  new_enemy->frame_h = frame_h;          // Altura de cada frame na spritesheet
  new_enemy->hb_width = hb_width;        // Largura da hitbox (área de colisão)
  new_enemy->hb_height = hb_height;      // Altura da hitbox (área de colisão)
  new_enemy->hb_x =
      (width - new_enemy->hb_width) / 2;  // Centraliza a hitbox horizontalmente
  new_enemy->hb_y = height - hb_height;   // Alinha a hitbox à base do sprite
  new_enemy->ativo = ativo;  // 1 = inimigo visível e ativo, 0 = inativo

  return new_enemy;  // Retorna o ponteiro para o inimigo criado
}

// Atualiza a movimentação do inimigo a cada frame
void enemy_movimentation(Enemy* inimigo, Platform** platforms, int count) {
  if (!inimigo || !platforms) {
    return;
  }

  // Laser é tratado separadamente — não tem física, só timer
  if (inimigo->movimento == 2) {
    inimigo->laser_timer++;  // Avança o contador de tempo do laser

    if (!inimigo->laser_ligado) {
      // Fase de aviso: pisca por 60 frames antes de ligar
      if (inimigo->laser_timer < 60) {
        inimigo->frame =
            (inimigo->laser_timer / 5) % 2;  // Alterna 0 e 1 a cada 5 frames
      } else {
        // Passou 60 frames de aviso: liga o laser e reseta o timer
        inimigo->laser_ligado = 1;
        inimigo->laser_timer = 0;
      }
    } else {
      // Laser ligado: fica ativo por 90 frames e depois apaga
      if (inimigo->laser_timer >= 90) {
        inimigo->laser_ligado = 0;  // Desliga o laser
        inimigo->laser_timer = 0;   // Reseta o timer para reiniciar o ciclo
      }
    }
    return;  // Laser não tem movimento físico, encerra aqui
  }

  if (inimigo->movel == 0) {
    return;  // Inimigo estático: nada a fazer
  }

  if (inimigo->movimento == 0) {
    // Movimento horizontal com gravidade

    if (inimigo->no_chao == 0) {
      inimigo->vel_y += GRAVIDADE;  // Aplica gravidade quando está no ar
    }
    inimigo->y += inimigo->vel_y;  // Move verticalmente pela gravidade

    if (inimigo->vel_x == 0) {
      inimigo->vel_x = ENEMY_STEP;  // Começa se movendo para a direita
    }
    inimigo->x += inimigo->vel_x;  // Aplica o movimento horizontal

    // Patrulha: inverte direção ao atingir os limites do spawn
    if (inimigo->x > inimigo->spawn_x + ENEMY_RANGE) {
      inimigo->vel_x =
          -ENEMY_STEP;  // Passou do limite direito: vai para esquerda
    } else if (inimigo->x < inimigo->spawn_x - ENEMY_RANGE) {
      inimigo->vel_x =
          ENEMY_STEP;  // Passou do limite esquerdo: vai para direita
    }

  } else {
    // Movimento vertical (ex: inimigo voador)

    if (inimigo->vel_y == 0) {
      inimigo->vel_y = ENEMY_STEP;  // Começa se movendo para baixo
    }
    inimigo->y += inimigo->vel_y;  // Aplica o movimento vertical

    // Patrulha vertical: inverte direção ao atingir os limites do spawn
    if (inimigo->y > inimigo->spawn_y + ENEMY_RANGE) {
      inimigo->vel_y = -ENEMY_STEP;  // Passou do limite inferior: sobe
    } else if (inimigo->y < inimigo->spawn_y - ENEMY_RANGE) {
      inimigo->vel_y = ENEMY_STEP;  // Passou do limite superior: desce
    }
  }

  // Verifica colisão com plataformas apenas para inimigos horizontais
  if (inimigo->movimento == 0) {
    inimigo->no_chao = 0;  // Assume que está no ar até provar o contrário

    for (int i = 0; i < count; i++) {
      int colisao = platform_check_collision(platforms[i], (Entity*)inimigo);
      if (colisao == TOP) {
        // Pousou em cima de uma plataforma: corrige posição e para a queda
        inimigo->y = platforms[i]->y - inimigo->hb_height - inimigo->hb_y;
        inimigo->vel_y = 0;
        inimigo->no_chao = 1;
      }
    }
  }
}

// Desenha a hitbox verde do inimigo (útil para debug)
void hitbox_draw_e(Enemy* inimigo) {
  if (!inimigo) {
    return;
  }

  al_draw_rectangle(
      inimigo->x + inimigo->hb_x,                       // X inicial da hitbox
      inimigo->y + inimigo->hb_y,                       // Y inicial da hitbox
      inimigo->x + inimigo->hb_x + inimigo->hb_width,   // X final da hitbox
      inimigo->y + inimigo->hb_y + inimigo->hb_height,  // Y final da hitbox
      al_map_rgb(0, 255, 0),                            // Cor verde
      2.0f                                              // Espessura da borda
  );
}

// Desenha o inimigo na tela
void enemy_draw(Enemy* inimigo) {
  if (!inimigo || !inimigo->ativo) {
    return;  // Não desenha se o inimigo não existe ou está inativo
  }

  // Laser tem desenho próprio com retângulos coloridos
  if (inimigo->movimento == 2) {
    if (!inimigo->laser_ligado) {
      // Fase de aviso: desenha amarelo transparente quando frame == 1
      if (inimigo->frame == 1) {
        al_draw_filled_rectangle(
            inimigo->x, inimigo->y, inimigo->x + inimigo->width,
            inimigo->y + inimigo->height,
            al_map_rgba(255, 255, 0, 80));  // Amarelo semi-transparente
      }
    } else {
      // Laser ativo: núcleo vermelho sólido com brilho nas bordas
      al_draw_filled_rectangle(inimigo->x, inimigo->y,
                               inimigo->x + inimigo->width,
                               inimigo->y + inimigo->height,
                               al_map_rgb(255, 0, 0));  // Núcleo vermelho opaco

      al_draw_filled_rectangle(
          inimigo->x, inimigo->y - 2,  // 2px acima para simular brilho
          inimigo->x + inimigo->width,
          inimigo->y + inimigo->height + 2,  // 2px abaixo para simular brilho
          al_map_rgba(255, 100, 100, 80));   // Vermelho claro semi-transparente
    }
    return;  // Laser não usa sprite, encerra aqui
  }

  // Avança o frame da animação
  inimigo->frame += 0.3f;
  if (inimigo->frame >= inimigo->frame_count) {
    inimigo->frame = 0;  // Reinicia a animação no último frame
  }

  if (inimigo->frame_count == 1) {
    // Sprite estático: desenha o bitmap inteiro escalado para o tamanho do
    // inimigo
    al_draw_scaled_bitmap(
        inimigo->sprite, 0, 0,   // Origem no bitmap: canto superior esquerdo
        inimigo->frame_w,        // Largura do recorte no bitmap
        inimigo->frame_h,        // Altura do recorte no bitmap
        inimigo->x, inimigo->y,  // Posição na tela
        inimigo->width, inimigo->height,  // Tamanho final na tela
        0);                               // Sem espelhamento
  } else {
    // Sprite animado: recorta o frame atual da spritesheet horizontal
    al_draw_scaled_bitmap(
        inimigo->sprite, inimigo->frame_w * (int)inimigo->frame,
        0,                                // Avança X pelo índice do frame
        inimigo->frame_w,                 // Largura de um frame
        inimigo->frame_h,                 // Altura de um frame
        inimigo->x, inimigo->y,           // Posição na tela
        inimigo->width, inimigo->height,  // Tamanho final na tela
        inimigo->vel_x > 0 ? ALLEGRO_FLIP_HORIZONTAL
                           : 0);  // Espelha se vai para direita
  }
}

// Libera a memória do inimigo e destrói seu bitmap
void enemy_destroy(Enemy* inimigo) {
  if (!inimigo) {
    return;
  }

  al_destroy_bitmap(inimigo->sprite);  // Libera a textura do Allegro
  free(inimigo);                       // Libera a struct
}

// Verifica se o inimigo colidiu com o player e aplica dano
void enemy_check_player_collision(Enemy* inimigo, Player* player) {
  if (!inimigo || !player) {
    return;
  }

  player_invencibility(
      player);  // Decrementa o timer de invencibilidade do player

  // Laser só causa dano quando está ligado
  if (inimigo->movimento == 2 && !inimigo->laser_ligado) {
    return;
  }

  // Calcula os limites da hitbox do player
  float p_left = player->x + player->hb_x;
  float p_right = p_left + player->hb_width;
  float p_top = player->y + player->hb_y;
  float p_bottom = p_top + player->hb_height;

  // Calcula os limites da hitbox do inimigo
  float e_left = inimigo->x + inimigo->hb_x;
  float e_right = e_left + inimigo->hb_width;
  float e_top = inimigo->y + inimigo->hb_y;
  float e_bottom = e_top + inimigo->hb_height;

  // Verifica sobreposição nos dois eixos (AABB)
  int colidiu = p_right > e_left && p_left < e_right && p_bottom > e_top &&
                p_top < e_bottom;

  if (colidiu && !player->invencivel) {
    player->lives--;         // Reduz uma vida
    player->invencivel = 1;  // Ativa invencibilidade temporária
    player->invencivel_timer =
        INVENCIVEL_FRAMES;  // Reseta o timer de invencibilidade
  }
}

// Retorna 1 se o inimigo tem movimento habilitado, 0 se é estático
int enemy_is_movable(Enemy* enemy) {
  if (!enemy) {
    return -1;
  }

  if (enemy->movel) {
    return 1;
  } else {
    return 0;
  }
}

// Ativa o inimigo quando o player chega na posição de gatilho
void enemy_activate_on_trigger(Enemy* inimigo, Player* player,
                               float trigger_x) {
  if (!inimigo || !player) {
    return;
  }

  if (!inimigo->ativo && player->x >= trigger_x) {
    inimigo->ativo = 1;            // Torna o inimigo visível e ativo
    inimigo->movel = 1;            // Habilita o movimento
    inimigo->vel_x = -ENEMY_STEP;  // Começa se movendo para a esquerda
  }
}