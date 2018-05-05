#include "keyboard.h"
#include "character.h"
#include "graph.h"
#include "level.h"
#include <stdio.h>


void main(void)
{
  character_t *main_character,*c;
  keyboard_key_t k;
  int32_t p[2]={10000,30000};

  level_init(1);

  graph_init();

  main_character=character_init_main(KID);  
  motion_set_pos(&main_character->body,p);
  
  c=character_init(GUARD);
  p[0]=30000;p[1]=20000;
  motion_set_pos(&c->body,p);
  
  c=character_init(VIZIR);
  p[0]=80000;p[1]=20000;
  motion_set_pos(&c->body,p);

  while(1)
    {
      c=character_get_list();
      while(c)
	{
	  if(c==character_get_main())
	    k=keyboard_check();
	  else
	    k=NONE;
	  	  
	  /*// an "artificial stupidity" example :D 
	    if(main_character->body.pos[0] < c->body.pos[0])
	    k=LEFT;
	  else if(main_character->body.pos[0] > c->body.pos[0])
	    k=RIGHT;
	  */
	  motion_move_body(&c->body,k);
	    
	  c=c->next;
	}
      graph_update();
    }
  

}
