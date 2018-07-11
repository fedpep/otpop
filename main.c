#include "keyboard.h"

#include "types.h"
#include "character.h"
#include "graph.h"

#include "level.h"
#include "ai.h"
#include "time.h"



//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif

void main(void)
{
  character_t *c;
  int32_t p[2];
  float v[2];
  uint32_t t,t_last=0;

  level_init(1);

  graph_init();

  c=character_init_main(KID);  
  p[0]=4000;p[1]=40000;
  motion_set_pos(&c->body,p);
  v[0]=40;v[1]=0;
  motion_set_vel(&c->body,v);
  
  
  c=character_init(GUARD);
  p[0]=30000;p[1]=20000;
  motion_set_pos(&c->body,p);
  
  /*
  c=character_init(GUARD);
  p[0]=80000;p[1]=20000;
  motion_set_pos(&c->body,p);
  */

  while(1)
    {
      t=time_get_now();
      if(t-t_last<60) 
	{
	  time_delay(10);
	  continue;
	}
      t_last=t;

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
	  
	  //character_state_check(c, t);

	  motion_move_body(&c->body, t);

	  PRINTF("-------%lx-main=%d-----\n",(long unsigned int) c,c==character_get_main());
	  PRINTF("- life=%d\n", c->life);
	  PRINTF("- state=%d\n", c->state);
	  c=c->next;
	}
      graph_update();
    }
  

}
