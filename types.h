#ifndef _TYPES_H_
#define _TYPES_H_

#include <SDL.h>

#define NONE  0 
#define UP    0x01  // when the shift amount is even it is a +
#define DOWN  0x02  // when the shift amount is even it is a -
#define RIGHT 0x04
#define LEFT  0x08

#define SHIFT  0x10

typedef uint32_t keyboard_key_t;


typedef enum
{
  CTRL_ACC=0,
  CTRL_VEL,
  CTRL_POS,
} control_t;



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
  MOTION_STATE_CLIMB_UP_L,
  MOTION_STATE_CLIMB_UP_R,
  MOTION_STATE_CLIMB_DOWN_L,
  MOTION_STATE_CLIMB_DOWN_R,
  MOTION_STATE_FALL_L,
  MOTION_STATE_FALL_R,
} body_state_t;

typedef struct
{
  control_t ctrl;
  int32_t pos[2];
  float vel[2];
  float acc[2];
  int32_t dim[2];
  int32_t mass;
  uint8_t on_constraint;
  body_state_t state;
  uint16_t clock;
  uint8_t suspend_dynamics;
  uint32_t last_t;
  keyboard_key_t key_pressed;
  uint32_t last_k_t;
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




#endif
