#include <stdlib.h>
#include "character.h"

character_t* character_init(character_kind_t kind)
{
  character_t *c;
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  character_set_pos(c,10000,30000);
  character_set_speed(c,0,0);
  c->life=100;
  c->pose_id=0;
  c->on_constraint=0;
  return c;
}

void character_set_pos(character_t *c, float x, float y)
{
  c->pos[0]=x;
  c->pos[1]=y;
}

void character_set_speed(character_t *c, float x_dot, float y_dot)
{
  c->pos_dot[0]=x_dot;
  c->pos_dot[1]=y_dot;
}
