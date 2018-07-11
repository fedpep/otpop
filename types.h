#ifndef _TYPES_H_
#define _TYPES_H_

#include <SDL.h>

#define NONE  0 
#define UP    0x01  // when the shift amount is even it is a +
#define DOWN  0x02  // when the shift amount is even it is a -
#define RIGHT 0x04
#define LEFT  0x08

#define SHIFT  0x10
#define CTRL  0x20
#define P_BUTTON  0x40


#define ON_A_FLOOR 0x01
#define ON_A_WALL  0x02
#define GOT_A_HIT    0x10

#define IS_HIT(BP) ((BP)->event & GOT_A_HIT)
#define SET_HIT(BP) ((BP)->event |= GOT_A_HIT)
#define CLEAR_HIT(BP) ((BP)->event &= ~GOT_A_HIT)

typedef uint32_t keyboard_key_t;

typedef enum {
  CHR_STATE_STAND_L,
  CHR_STATE_STAND_R,
  CHR_STATE_RUN_L,
  CHR_STATE_RUN_R,
  CHR_STATE_BRAKE_L,
  CHR_STATE_BRAKE_R,
  CHR_STATE_INVERT_L2R,
  CHR_STATE_INVERT_R2L,
  CHR_STATE_CHANGE_DIR_L2R,
  CHR_STATE_CHANGE_DIR_R2L,
  CHR_STATE_JUMP_L,
  CHR_STATE_JUMP_R,
  CHR_STATE_RUN_JUMP_L,
  CHR_STATE_RUN_JUMP_R,
  CHR_STATE_JUMP_FWD_L,
  CHR_STATE_JUMP_FWD_R,
  CHR_STATE_CROUCH_L,
  CHR_STATE_CROUCH_R,
  CHR_STATE_STEP_L,
  CHR_STATE_STEP_R,
  CHR_STATE_STEP_DANG_L,
  CHR_STATE_STEP_DANG_R,
  CHR_STATE_CLIMB_UP_L,
  CHR_STATE_CLIMB_UP_R,
  CHR_STATE_CLIMB_DOWN_L,
  CHR_STATE_CLIMB_DOWN_R,
  CHR_STATE_FALL_L,
  CHR_STATE_FALL_R,
  CHR_STATE_HANG_L,
  CHR_STATE_HANG_R,
  CHR_STATE_FIGHT_IN_GUARD_L,
  CHR_STATE_FIGHT_IN_GUARD_R,
  CHR_STATE_FIGHT_UNSHEATHE_L,
  CHR_STATE_FIGHT_UNSHEATHE_R,
  CHR_STATE_FIGHT_SHEATHE_L,
  CHR_STATE_FIGHT_SHEATHE_R,
  CHR_STATE_FIGHT_FWD_L,
  CHR_STATE_FIGHT_FWD_R,
  CHR_STATE_FIGHT_BACK_L,
  CHR_STATE_FIGHT_BACK_R,
  CHR_STATE_FIGHT_ATTACK_L,
  CHR_STATE_FIGHT_ATTACK_R,
  CHR_STATE_GET_POTION_L,
  CHR_STATE_GET_POTION_R,
  CHR_STATE_GET_HIT_L,
  CHR_STATE_GET_HIT_R,
  CHR_STATE_GET_HIT_TO_DEATH_L,
  CHR_STATE_GET_HIT_TO_DEATH_R,
  CHR_STATE_DEAD_L,
  CHR_STATE_DEAD_R,

} chr_state_t;

typedef struct
{
  int32_t pos[2];
  float vel[2];
  float acc[2];
  int32_t dim[2];
  int32_t mass;
  uint8_t event;
  uint8_t suspend_dynamics;
  uint32_t last_t;
} body_t;

typedef enum
{
  KID=0,
  SKELETON,
  GUARD,
  VIZIR,
  MOUSE,
  PRINCESS,
} character_kind_t;


typedef struct
{
  SDL_Surface *fig_surf;
  SDL_Rect fig_rect;
  uint32_t last_t;
  SDL_Rect *clips;
  uint32_t clips_size;
  uint32_t clip_start_index;
  uint32_t clip_current_index;
} figure_t;


typedef struct character_struct
{
  body_t body; 
  character_kind_t kind;
  uint8_t life;
  figure_t* figure_ptr;
  chr_state_t state;
  uint16_t clock;
  keyboard_key_t key_pressed;
  struct character_struct *next;
} character_t;

typedef struct constraint_struct
{
  int32_t p_start[2];
  int32_t p_end[2];
  struct constraint_struct *next;
} constraint_t;



#endif
