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


#define ON_A_FLOOR        0x01
#define ON_A_WALL         0x02
#define DIRECTION_MASK   0x04
#define GOT_A_HIT         0x80


#define DIRECTION_IS_RIGHT(BP)       ((BP)->flags & DIRECTION_MASK)
#define DIRECTION_IS_LEFT(BP)        (!DIRECTION_IS_RIGHT(BP))
#define DIRECTION_TOGGLE(BP)         {(BP)->flags ^= DIRECTION_MASK;}
#define DIRECTION_SET_LEFT(BP)         {(BP)->flags &= ~DIRECTION_MASK;}
#define DIRECTION_SET_RIGHT(BP)         {(BP)->flags |= DIRECTION_MASK;}

#define IS_HIT(BP)          ((BP)->flags & GOT_A_HIT)
#define SET_HIT(BP)         ((BP)->flags |= GOT_A_HIT)
#define CLEAR_HIT(BP)       ((BP)->flags &= ~GOT_A_HIT)

typedef uint32_t keyboard_key_t;

typedef enum {
    DIR_LEFT=0,
    DIR_RIGHT
} direction_t;

typedef enum {
  CHR_STATE_STAND,
  CHR_STATE_RUN,
  CHR_STATE_BRAKE,
  CHR_STATE_RUN_CHANGE_DIR,
  CHR_STATE_CHANGE_DIR,
  CHR_STATE_JUMP,
  CHR_STATE_RUN_JUMP,
  CHR_STATE_JUMP_FWD,
  CHR_STATE_CROUCH,
  CHR_STATE_STEP,
  CHR_STATE_STEP_DANG,
  CHR_STATE_CLIMB_UP,
  CHR_STATE_CLIMB_DOWN,
  CHR_STATE_FALL,
  CHR_STATE_HANG,
  CHR_STATE_FIGHT_IN_GUARD,
  CHR_STATE_FIGHT_UNSHEATHE,
  CHR_STATE_FIGHT_SHEATHE,
  CHR_STATE_FIGHT_FWD,
  CHR_STATE_FIGHT_BACK,
  CHR_STATE_FIGHT_ATTACK,
  CHR_STATE_GET_POTION,
  CHR_STATE_GET_HIT,
  CHR_STATE_GET_HIT_TO_DEATH,
  CHR_STATE_DEAD,
  CHR_SIZE_STATES,
} chr_state_t;

typedef struct
{
  int32_t pos[2];
  float vel[2];
  float acc[2];
  int32_t dim[2];
  int32_t mass;
  uint8_t flags;
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
  const SDL_Rect *clips;
  uint32_t clips_size;
  uint32_t clip_start_index;
  uint32_t clip_current_index;
  void (*get_clip_indexes)(chr_state_t s, const uint16_t **ind);
  uint32_t l2r_x_offset;
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
