#include "keyboard.h"
#include "character.h"
#include "graph.h"
#include "level.h"
#include "ai.h"

#include <stdio.h>


void main(void)
{
  character_t *c;
  int32_t p[2];

  level_init(1);

  graph_init();

  c=character_init_main(KID);  
  p[0]=10000;p[1]=30000;
  motion_set_pos(&c->body,p);
  
  c=character_init(GUARD);
  p[0]=30000;p[1]=20000;
  motion_set_pos(&c->body,p);
  
  c=character_init(GUARD);
  p[0]=80000;p[1]=20000;
  motion_set_pos(&c->body,p);
  

  while(1)
    {
      c=character_get_list();
      while(c)
	{
	  
	  if(c==character_get_main())
	    {
	      c->body.key_pressed=keyboard_check();
	    }
	  else
	    {
	      ai_command(c);
	    }
	  
	  character_state_check(c);

	  motion_move_body(&c->body);
	 
	  c=c->next;
	}
      graph_update();
    }
  

}
