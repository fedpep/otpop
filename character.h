#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <stdint.h>
#include "motion.h"


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
  FIGHT,
  DEAD,
  IDLE_R,
} character_state_t;

typedef struct character_struct
{
  body_t body; 
  character_state_t state;
  character_kind_t kind;
  uint8_t life;
  void* figure_ptr;
  struct character_struct *next;
} character_t;


character_t* character_init(character_kind_t kind);
character_t* character_init_main(character_kind_t kind);
character_t *character_get_list(void);
character_t* character_get_main(void);
void character_state_check(character_t *c);

#endif
