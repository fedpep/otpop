#include <stdlib.h>
#include "character.h"
#include "keyboard.h"
#include "ai.h"

//#define DEBUG

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

  PRINTF("chr %llx: dist=%d\n",c, dist);

  if(main_character->body.pos[0]>c->body.pos[0])
    {
      if(dist>target_dist)
	{
	  c->key_pressed=RIGHT;
	}
      /*else if(0&&dist<target_dist-6000)
	{
	  c->key_pressed=LEFT;
	  }*/
    }
  else
    {
      if(dist>target_dist)
	{
	  c->key_pressed=LEFT;
	}
      /*else if(dist<target_dist-6000)
	{
	  c->key_pressed=RIGHT;
	  }*/
    }
  
  return (dist<=target_dist);
}

void ai_command(character_t *c)
{
  character_t *main_character;
  uint32_t dist_target;
  uint8_t (*motion_body_close)(body_t *b1, body_t *b2, uint32_t distance);
  c->key_pressed=NONE;
  main_character=character_get_main();

  switch(c->state)
    {
    case CHR_STATE_STAND:
      motion_body_close=(DIRECTION_IS_LEFT(&c->body))?(motion_body_close_l):(motion_body_close_r);
      if(main_character->state != CHR_STATE_DEAD && motion_body_close(&c->body, &main_character->body, 20000))
	{
	  c->key_pressed|=CTRL;
	}
      break;
      
    case CHR_STATE_FIGHT_IN_GUARD:
      
      motion_body_close=(DIRECTION_IS_LEFT(&c->body))?(motion_body_close_l):(motion_body_close_r);
      if(main_character->state == CHR_STATE_DEAD || !motion_body_close(&c->body, &main_character->body, 20000))
	{
	  c->key_pressed|=DOWN;
	}
      else
	{
	  ai_approach_main_character(c,15000);
	}

      if(motion_body_close(&c->body, &main_character->body, 7000))
	{
	  if(rand()%100<40)
	    c->key_pressed|=CTRL;
	}
      else
	{
	  if(rand()%100<20)
	    c->key_pressed|=(DIRECTION_IS_LEFT(&c->body))?(LEFT):(RIGHT);
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



