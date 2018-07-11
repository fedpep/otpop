#include <stdlib.h>
#include "character.h"
#include "keyboard.h"
#include "ai.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif

#define ABS(x)  (((x)>=0)?(x):(-(x)))


static uint8_t ai_approach_main_character(character_t *c, uint32_t target_dist)
{
  character_t *main_character=character_get_main();
  uint32_t dist=ABS(main_character->body.pos[0]-c->body.pos[0]);

  PRINTF("dist=%d\n",dist);

  if(main_character->body.pos[0]>c->body.pos[0])
    {
      if(dist>target_dist)
	{
	  c->body.key_pressed=RIGHT;
	}
      else if(dist<target_dist-3000)
	{
	  c->body.key_pressed=LEFT;
	}
    }
  else
    {
      if(dist>target_dist)
	{
	  c->body.key_pressed=LEFT;
	}
      else if(dist<target_dist-3000)
	{
	  c->body.key_pressed=RIGHT;
	}
    }
  
  return (dist<=target_dist);
}

void ai_command(character_t *c)
{
  character_t *main_character;
  uint32_t dist_target;

  c->body.key_pressed=NONE;
  main_character=character_get_main();

  switch(c->body.state)
    {
    case MOTION_STATE_STAND_L:
      if(motion_body_close_l(&c->body, &main_character->body, 20000))
	{
	  c->body.key_pressed|=CTRL;
	}
      break;

    case MOTION_STATE_STAND_R:
      if(motion_body_close_r(&c->body, &main_character->body, 20000))
	{
	  c->body.key_pressed|=CTRL;
	}
      break;
      
    case MOTION_STATE_FIGHT_IN_GUARD_L:
      if(!motion_body_close_l(&c->body, &main_character->body, 20000))
	{
	  c->body.key_pressed|=DOWN;
	}
      break;
    case MOTION_STATE_FIGHT_IN_GUARD_R:
      if(!motion_body_close_r(&c->body, &main_character->body, 20000))
	{
	  c->body.key_pressed|=DOWN;
	}
      break;
    }
  /*
  switch(c->state)
    {
    case IN_GUARD:
      
      dist_target=8000;
      switch(main_character->state)
	{
	case IN_GUARD:
	case ATTACK:
	case DEFENSE:
	  if((rand()%100)<5)
	    {
	      dist_target=4000;
	    }
	  //printf("dist_tgt %d\n",dist_target);
	  ai_approach_main_character(c,dist_target);
	  break;
	case IDLE:
	case IDLE_R:
	  ai_approach_main_character(c,3000);
	  break;
	}
      break;      
    }
  */
}



