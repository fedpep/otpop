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
  p[0]=0;p[1]=40000;
  v[0]=30;v[1]=0;
  character_set_initial_state(c, p, v, DIR_RIGHT);
  
  c=character_init(GUARD);
  p[0]=36000;p[1]=20000;
  v[0]=0;v[1]=0;
  character_set_initial_state(c, p, v, DIR_RIGHT);

  c=character_init(GUARD);
  p[0]=15000;p[1]=70000;
  v[0]=0;v[1]=0;
  character_set_initial_state(c, p, v, DIR_LEFT);
  

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
	  time_delay(20);
	  continue;
	}
      t_last=t;

      c=character_get_list();
      while(c)
	{
	  
	  if(c==character_get_main())
	    {
	      c->key_pressed=keyboard_check();
	    }
	  else
	    {
	      ai_command(c);
	    }
	  

	  character_state_tick(c);
	  motion_move_body(&c->body, t);

	  PRINTF("-------%lx-main=%d-----\n",(long unsigned int) c,c==character_get_main());
	  PRINTF("- life=%d\n", c->life);
	  PRINTF("- state=%d\n", c->state);
	  c=c->next;
	}
      graph_update();
    }
  

}
