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

void ai_command(character_t *c)
{
  character_t *main_character;
  int32_t dist;

  main_character=character_get_main();

  switch(c->state)
    {
    case FIGHT:
      dist=ABS(main_character->body.pos[0]-c->body.pos[0]);
      //PRINTF("dist=%d\n",dist);
      if((dist>3000 && main_character->state==FIGHT)
	 || (dist>0 && (main_character->state==IDLE_R || main_character->state==IDLE)))
	{
	  if(main_character->body.pos[0]>c->body.pos[0])
	    {
	      c->body.key_pressed=RIGHT;
	    }
	  if(main_character->body.pos[0]<c->body.pos[0])
	    {
	      c->body.key_pressed=LEFT;
	    }
	}
      /*else if(dist<1000 && main_character->state==FIGHT)
	{
	  if(main_character->body.pos[0]>c->body.pos[0])
	    {
	      c->body.key_pressed=LEFT;
	    }
	  if(main_character->body.pos[0]<c->body.pos[0])
	    {
	      c->body.key_pressed=RIGHT;
	    }
	    }*/
      break;
      
      
    }
  
}



