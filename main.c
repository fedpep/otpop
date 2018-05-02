#include "keyboard.h"
#include "character.h"
#include "graph.h"
#include "level.h"
#include <stdio.h>


void main(void)
{
  keyboard_key_t k;
  int32_t p[2]={10000,30000};

  level_init(1);

  graph_init();

  character_t* c=character_init(KID);
  character_set_main(c);
  motion_set_pos(&c->body,p);
  
  c=character_init(GUARD);
  p[0]=30000;p[1]=20000;
  motion_set_pos(&c->body,p);

  while(1)
    {
      c=character_get_list();
      while(c)
	{
	  
	  k=0;
	  if(character_is_main(c))
	    {
	      k=keyboard_check();
	    }

	  motion_move_body(&c->body,k);
	    
	  c=c->next;
	}
      graph_update();
    }
  

}
