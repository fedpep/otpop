#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <stdint.h>
#include "motion.h"

typedef enum
{
  CHICKEN=0,
  SQUIRREL,
  DOG,
  RABBIT
} character_kind_t;


typedef struct
{
  body_t body; 
  uint16_t pose_id;
  character_kind_t kind;
  uint8_t life;
} character_t;


character_t* character_init(character_kind_t kind);

#endif
