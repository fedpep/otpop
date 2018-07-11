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
  MOTION_STATE_STAND_L,
  MOTION_STATE_STAND_R,
  MOTION_STATE_RUN_L,
  MOTION_STATE_RUN_R,
  MOTION_STATE_BRAKE_L,
  MOTION_STATE_BRAKE_R,
  MOTION_STATE_INVERT_L2R,
  MOTION_STATE_INVERT_R2L,
  MOTION_STATE_CHANGE_DIR_L2R,
  MOTION_STATE_CHANGE_DIR_R2L,
  MOTION_STATE_JUMP_L,
  MOTION_STATE_JUMP_R,
  MOTION_STATE_RUN_JUMP_L,
  MOTION_STATE_RUN_JUMP_R,
  MOTION_STATE_JUMP_FWD_L,
  MOTION_STATE_JUMP_FWD_R,
  MOTION_STATE_CROUCH_L,
  MOTION_STATE_CROUCH_R,
  MOTION_STATE_STEP_L,
  MOTION_STATE_STEP_R,
  MOTION_STATE_STEP_DANG_L,
  MOTION_STATE_STEP_DANG_R,
  MOTION_STATE_CLIMB_UP_L,
  MOTION_STATE_CLIMB_UP_R,
  MOTION_STATE_CLIMB_DOWN_L,
  MOTION_STATE_CLIMB_DOWN_R,
  MOTION_STATE_FALL_L,
  MOTION_STATE_FALL_R,
  MOTION_STATE_HANG_L,
  MOTION_STATE_HANG_R,
  MOTION_STATE_FIGHT_IN_GUARD_L,
  MOTION_STATE_FIGHT_IN_GUARD_R,
  MOTION_STATE_FIGHT_UNSHEATHE_L,
  MOTION_STATE_FIGHT_UNSHEATHE_R,
  MOTION_STATE_FIGHT_SHEATHE_L,
  MOTION_STATE_FIGHT_SHEATHE_R,
  MOTION_STATE_FIGHT_FWD_L,
  MOTION_STATE_FIGHT_FWD_R,
  MOTION_STATE_FIGHT_BACK_L,
  MOTION_STATE_FIGHT_BACK_R,
  MOTION_STATE_FIGHT_ATTACK_L,
  MOTION_STATE_FIGHT_ATTACK_R,
  MOTION_STATE_GET_POTION_L,
  MOTION_STATE_GET_POTION_R,
  MOTION_STATE_GET_HIT_TO_DEATH_L,
  MOTION_STATE_GET_HIT_TO_DEATH_R,
  MOTION_STATE_DEAD_L,
  MOTION_STATE_DEAD_R,

} body_state_t;

typedef struct
{
  int32_t pos[2];
  float vel[2];
  float acc[2];
  int32_t dim[2];
  int32_t mass;
  uint8_t event;
  body_state_t state;
  uint16_t clock;
  uint8_t suspend_dynamics;
  uint32_t last_t;
  keyboard_key_t key_pressed;
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


typedef enum
{
  IDLE=0,
  IN_GUARD,
  ATTACK,
  DEFENSE,
  DEAD,
  HIT,
  IDLE_R,
} character_state_t;




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
  character_state_t state;
  character_kind_t kind;
  uint8_t life;
  figure_t* figure_ptr;
  struct character_struct *next;
} character_t;

typedef struct constraint_struct
{
  int32_t p_start[2];
  int32_t p_end[2];
  struct constraint_struct *next;
} constraint_t;



#endif
