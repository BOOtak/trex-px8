#include <arch/px8.h>
#include <graphics.h>
#include <string.h>

#include "cactus_sprite.h"
#include "dino_sprites.h"

#define SCREEN_BLOCK_WIDTH 60
#define OP_DEFINE_SPRITE 0x20
#define OP_DRAW_SPRITE 0x23

#define KEY_EXIT 0x61

#define FRAME_1_IDX 1
#define FRAME_2_IDX 2
#define CACTUS_SPRITE_1_IDX 3
#define CACTUS_SPRITE_2_IDX 4
#define DINO_STANDING 5

#define DINO_X 2
#define DINO_Y 39

#define CACTUS_Y 40

#define uchar unsigned char

#define MAX_SPRITE_CMD_SIZE 255

#define AABB(x1, y1, x2, y2, w1, h1, w2, h2)                            \
    ((((x1) + (w1) > (x2)) && ((x1) < (x2))) || (((x2) + (w2) > (x1)) && ((x2) < (x1)))) && \
        ((y1) + (h1) > (y2))

extern unsigned char px_getk() __z88dk_fastcall;

typedef struct {
  unsigned char op;
  unsigned char sprite_index;
  unsigned char width;
  unsigned char height;
} define_sprite_header_t;

typedef struct {
  unsigned char op;
  unsigned char x;
  unsigned char y;
  unsigned char sprite_index;
} draw_sprite_cmd_t;

typedef struct {
  short cmd;
  short cmd_len;
  short res;
  short res_len;
} pack_t;

typedef enum { RUNNING, JUMPING } state_t;

typedef enum { DIR_UP = -3, DIR_DOWN = 3 } jump_dir_t;

char sprite_cmd_buf[MAX_SPRITE_CMD_SIZE];
draw_sprite_cmd_t draw_dino_cmd = {OP_DRAW_SPRITE, 5, 42, 1};
draw_sprite_cmd_t draw_cactus_cmd_1 = {OP_DRAW_SPRITE, 5, 42, 1};
draw_sprite_cmd_t draw_cactus_cmd_2 = {OP_DRAW_SPRITE, 5, 42, 1};
pack_t pack;

void define_sprite(uchar index, uchar width, uchar height, uchar* data) {
  if ((width * height) >
      (MAX_SPRITE_CMD_SIZE - sizeof(define_sprite_header_t))) {
    return;
  }

  define_sprite_header_t header;
  header.op = OP_DEFINE_SPRITE;
  header.sprite_index = index;
  header.width = width;
  header.height = height;

  memcpy(sprite_cmd_buf, header, sizeof(define_sprite_header_t));
  memcpy(&(sprite_cmd_buf[4]), data, width * height);

  char res;
  pack_t cmd;
  cmd.cmd = &sprite_cmd_buf;
  cmd.cmd_len = sizeof(define_sprite_header_t) + width * height;
  cmd.res = &res;
  cmd.res_len = 1;

  subcpu_call(cmd);
}

void define_sprites() {
  define_sprite(FRAME_1_IDX, DINO_SPRITE_WIDTH_BLOCK, DINO_SPRITE_HEIGHT,
                frame_1);
  define_sprite(FRAME_2_IDX, DINO_SPRITE_WIDTH_BLOCK, DINO_SPRITE_HEIGHT,
                frame_2);
  define_sprite(DINO_STANDING, DINO_SPRITE_WIDTH_BLOCK,
                DINO_SPRITE_JUMPING_HEIGHT, frame_3);
  define_sprite(CACTUS_SPRITE_1_IDX, CACTUS_SPRITE_WIDTH_BLOCK,
                CACTUS_SPRITE_HEIGHT, cactus_data_1);
  define_sprite(CACTUS_SPRITE_2_IDX, CACTUS_SPRITE_WIDTH_BLOCK,
                CACTUS_SPRITE_HEIGHT, cactus_data_2);
}

int main() {
  clg();
  define_sprites();

  char result;

  pack.cmd = &draw_dino_cmd;
  pack.cmd_len = sizeof(draw_dino_cmd);
  pack.res = &result;
  pack.res_len = 1;

  uchar* dino_index = &(draw_dino_cmd.sprite_index);
  uchar* dino_x = &(draw_dino_cmd.x);
  uchar* dino_y = &(draw_dino_cmd.y);

  uchar* cactus_1_x = &(draw_cactus_cmd_1.x);
  uchar* cactus_1_y = &(draw_cactus_cmd_1.y);
  uchar* cactus_1_index = &(draw_cactus_cmd_1.sprite_index);

  uchar* cactus_2_x = &(draw_cactus_cmd_2.x);
  uchar* cactus_2_y = &(draw_cactus_cmd_2.y);
  uchar* cactus_2_index = &(draw_cactus_cmd_2.sprite_index);

  *dino_x = DINO_X;
  *dino_y = DINO_Y;

  *cactus_1_y = CACTUS_Y;
  *cactus_2_y = CACTUS_Y;

  uchar should_run = 1;
  state_t state = RUNNING;
  uchar jump_max_heigh = 8;
  jump_dir_t jump_dir = DIR_UP;

  uchar anim_state = 0;

  uchar c_x1, c_x2;
  c_x1 = 0;
  c_x2 = 40;

  while (should_run) {
    switch (px_getk()) {
      case KEY_UP: {
        state = JUMPING;
        break;
      }
      case KEY_EXIT: {
        should_run = 0;
        break;
      }
    }

    switch (state) {
      case RUNNING: {
        anim_state = (anim_state + 1) & 0b11;
        *dino_index = (anim_state >> 1) + FRAME_1_IDX;
        break;
      }
      case JUMPING: {
        *dino_y = *dino_y + jump_dir;
        if (*dino_y < jump_max_heigh) {
          jump_dir = DIR_DOWN;
        }
        if (jump_dir == DIR_DOWN && *dino_y >= DINO_Y) {
          state = RUNNING;
          jump_dir = DIR_UP;
        }

        *dino_index = DINO_STANDING;
        break;
      }
    }

    c_x1++;
    c_x2++;
    *cactus_1_x = SCREEN_BLOCK_WIDTH - (c_x1 >> 1);
    if (c_x1 > SCREEN_BLOCK_WIDTH * 2 - 1) {
      c_x1 = 0;
    }
    *cactus_1_index = (c_x1 & 1) + CACTUS_SPRITE_1_IDX;
    pack.cmd = &draw_cactus_cmd_1;
    subcpu_call(&pack);
    *cactus_2_x = SCREEN_BLOCK_WIDTH - (c_x2 >> 1);
    if (c_x2 > SCREEN_BLOCK_WIDTH * 2 - 1) {
      c_x2 = 0;
    }

    *cactus_2_index = (c_x2 & 1) + CACTUS_SPRITE_1_IDX;
    pack.cmd = &draw_cactus_cmd_2;
    subcpu_call(&pack);

    pack.cmd = &draw_dino_cmd;
    subcpu_call(&pack);

    if (AABB(*dino_x, *dino_y, *cactus_1_x + CACTUS_CR_OFFSET_X, CACTUS_Y + CACTUS_CR_OFFSET_Y, DINO_CR_WIDTH, DINO_CR_HEIGHT, CACTUS_CR_WIDTH, 0)) {
      should_run = 0;
    }

    if (AABB(*dino_x, *dino_y, *cactus_2_x + CACTUS_CR_OFFSET_X, CACTUS_Y + CACTUS_CR_OFFSET_Y, DINO_CR_WIDTH, DINO_CR_HEIGHT, CACTUS_CR_WIDTH, 0)) {
      should_run = 0;
    }

  }

  int a = 0;
  while(px_getk() == 0);
  return 0;
}
