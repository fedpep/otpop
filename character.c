#include <stdlib.h>
#include "character.h"

static character_t *characters_list=NULL;
static character_t *main_character=NULL;


character_t* character_init(character_kind_t kind)
{
  character_t *c,*aux;
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  c->life=100;
  c->pose_id=0;

  motion_init_body(&c->body);
  
  c->next=characters_list;
  characters_list=c;

  return c;
}

void character_set_main(character_t *c)
{
  main_character=c;
}

character_t* character_get_main(void)
{
  return main_character;
}

uint8_t character_is_main(character_t *c)
{
  if(main_character==c)
    return 1;

  return 0;
}


character_t *character_get_list(void)
{
  return characters_list;
}
