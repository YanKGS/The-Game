#include "Platform.h"

#include "Enemy.h"
#include "Player.h"

// Cria uma nova plataforma com as dimensões e posição especificadas
// desaparece: 1 se a plataforma some ao ser tocada, 0 se é permanente
Platform* platform_create(float x, float y, float width, float height,
                          int desaparece) {
  Platform* new_platform = malloc(sizeof(Platform));
  if (!new_platform) {
    return NULL;
  }

  new_platform->x = x;
  new_platform->y = y;
  new_platform->width = width;
  new_platform->height = height;
  new_platform->colisao = 1;  // começa com colisão ativa
  new_platform->fixo = 0;     // não é fixo por padrão
  new_platform->desaparece = desaparece;
  new_platform->visivel = 1;  // começa visível
  new_platform->timer = 0;    // timer zerado
  new_platform->ativada = 0;  // ainda não foi tocada

  return new_platform;
}

// Desenha a plataforma na tela
// Se for do tipo "desaparece" e foi ativada, pisca antes de sumir
void platforms_draw(Platform* plat) {
  if (!plat) {
    return;
  }
  // não desenha se estiver invisível
  if (!plat->visivel) {
    return;
  }

  // efeito de piscar quando ativada (alterna a cada 4 frames)
  if (plat->desaparece && plat->ativada) {
    if ((int)plat->timer % 8 < 4) {
      return;
    }
  }

  al_draw_filled_rectangle(plat->x, plat->y, plat->x + plat->width,
                           plat->y + plat->height, al_map_rgb(12, 34, 34));
}

// Libera a memória alocada pela plataforma
void platform_destroy(Platform* platforms) { free(platforms); }

// Verifica a colisão entre uma plataforma e uma entidade (player ou inimigo)
// Retorna o lado da colisão: TOP, BOTTOM, LEFT, RIGHT ou NO_COLLISION
int platform_check_collision(Platform* platform, Entity* entidade) {
  if (!platform || !entidade) {
    return -1;
  }
  // sem colisão se plataforma estiver desativada
  if (!platform->colisao) {
    return NO_COLLISION;
  }

  float ent_x = entidade->x + entidade->hb_x;
  float ent_y = entidade->y + entidade->hb_y;
  float ent_width = entidade->hb_width;
  float ent_height = entidade->hb_height;

  // verifica se há sobreposição geral entre entidade e plataforma
  if (ent_x + ent_width <= platform->x ||
      ent_x >= platform->x + platform->width ||
      ent_y + ent_height <= platform->y ||
      ent_y >= platform->y + platform->height) {
    return NO_COLLISION;
  }

  // calcula a penetração em cada lado para determinar o lado da colisão
  float pen_top = ent_y + ent_height - platform->y;  // penetração pelo topo
  float pen_bottom =
      platform->y + platform->height - ent_y;        // penetração pelo fundo
  float pen_left = ent_x + ent_width - platform->x;  // penetração pela esquerda
  float pen_right =
      platform->x + platform->width - ent_x;  // penetração pela direita

  // o lado com menor penetração é o lado da colisão
  float min = pen_top;
  int side = TOP;

  if (pen_bottom < min) {
    min = pen_bottom;
    side = BOTTOM;
  }
  if (pen_left < min) {
    min = pen_left;
    side = LEFT;
  }
  if (pen_right < min) {
    min = pen_right;
    side = RIGHT;
  }

  return side;
}

// Atualiza o estado da plataforma que desaparece
// Ativada ao ser tocada pelo player, some após PLATAFORMA_TIMER frames
// e reaparece após PLATAFORMA_RESET frames
void platform_update(Platform* platform, Entity* entidade) {
  if (!platform || !entidade) {
    return;
  }

  if (!platform->desaparece) {
    return;
  }

  // verifica se o player está em cima da plataforma (margem de 5px)
  int player_em_cima =
      entidade->x + entidade->hb_x + entidade->hb_width > platform->x &&
      entidade->x + entidade->hb_x < platform->x + platform->width &&
      entidade->y + entidade->hb_y + entidade->hb_height >= platform->y &&
      entidade->y + entidade->hb_y + entidade->hb_height <= platform->y + 5;

  // ativa a plataforma ao ser tocada pela primeira vez
  if (player_em_cima && platform->visivel) {
    platform->ativada = 1;
  }

  // conta o tempo até sumir
  if (platform->ativada && platform->visivel) {
    platform->timer++;
    if (platform->timer >= PLATAFORMA_TIMER) {
      platform->visivel = 0;  // some
      platform->colisao = 0;  // desativa colisão
      platform->timer = 0;    // reseta timer
    }
  }

  // conta o tempo até reaparecer
  if (!platform->visivel) {
    platform->timer++;
    if (platform->timer >= PLATAFORMA_RESET) {
      platform->visivel = 1;  // reaparece
      platform->colisao = 1;  // reativa colisão
      platform->timer = 0;    // reseta timer
      platform->ativada = 0;  // reseta ativação
    }
  }
}