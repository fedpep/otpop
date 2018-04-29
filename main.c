#include "keyboard.h"
#include "character.h"
#include "graph.h"
#include "level.h"
#include <stdio.h>


void main(void)
{
  keyboard_key_t k;
  int32_t p[2]={10000,30000};

  character_t* c=character_init(SQUIRREL);
  motion_set_pos(&c->body,p);

  level_init(1);

  graph_init();
  
  while(1)
    {
      k=keyboard_check();
      if(motion_move_body(&c->body,k))
	graph_update(c);
    }
  

}
