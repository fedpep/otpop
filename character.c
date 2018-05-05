#include <stdlib.h>
#include "graph.h"
#include "character.h"


static character_t *characters_list=NULL;
static character_t *main_character=NULL;



static character_t* character_init_internal(character_kind_t kind)
{
  character_t *c,*aux;
  int32_t m=1;
  int32_t dim[2]={1,1};
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  c->life=100;
  c->pose_id=0;

  switch(c->kind)
    {
    case KID:
    case PRINCESS:
      m=1;
      dim[0]=10;
      dim[1]=170;
      break;
    case GUARD:
    case VIZIR:
      m=1;
      dim[0]=12;
      dim[1]=180;
      break;
    }
  motion_init_body(&c->body, dim, m);
  c->figure_ptr=graph_init_figure(c->kind);
  
  return c;
}

character_t* character_init(character_kind_t kind)
{
  character_t *c=character_init_internal(kind);
  c->next=characters_list;
  characters_list=c;
  return c;
}

character_t* character_init_main(character_kind_t kind)
{
  character_t *c=character_init(kind);
  main_character=c;
  return c;
}

character_t* character_get_main(void)
{
  return main_character;
}


character_t *character_get_list(void)
{
  return characters_list;
}
