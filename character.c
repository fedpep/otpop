#include <stdlib.h>
#include "character.h"


character_t* character_init(character_kind_t kind)
{
  float z[2]={0,0};

  character_t *c;
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  c->life=100;
  c->pose_id=0;

  motion_init_body(&c->body);


  return c;
}


