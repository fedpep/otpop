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


typedef struct character_struct
{
  body_t body; 
  uint16_t pose_id;
  character_kind_t kind;
  uint8_t life;
  struct character_struct *next;
} character_t;


character_t* character_init(character_kind_t kind);
character_t *character_get_list(void);
void character_set_main(character_t *c);
character_t* character_get_main(void);
uint8_t character_is_main(character_t *c);

#endif
