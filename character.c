#include <stdlib.h>
//#include "graph.h"
#include "character.h"
#include <stdio.h>

#define ABS(x)  (((x)>=0)?(x):(-(x)))

static character_t *characters_list=NULL;
static character_t *main_character=NULL;


static character_t* character_init_internal(character_kind_t kind)
{
  character_t *c,*aux;
  int32_t m=1;
  int32_t dim[2]={1,1};
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  c->life=3;
  c->state=IDLE;

  switch(c->kind)
    {
    case KID:
    case PRINCESS:
      m=1;
      dim[0]=10;
      dim[1]=170;
      break;
    case GUARD:
    case VIZIR:
      m=1;
      dim[0]=12;
      dim[1]=180;
      break;
    }
  motion_init_body(&c->body, dim, m);
  c->figure_ptr=graph_init_figure(c->kind);
  
  return c;
}

character_t* character_init(character_kind_t kind)
{
  character_t *c=character_init_internal(kind);
  c->next=characters_list;
  characters_list=c;
  return c;
}

character_t* character_init_main(character_kind_t kind)
{
  character_t *c=character_init(kind);
  main_character=c;
  return c;
}

character_t* character_get_main(void)
{
  return main_character;
}


character_t *character_get_list(void)
{
  return characters_list;
}


/*
static uint8_t character_close_to_each_other(character_t *c1, character_t *c2, uint32_t distance)
{
  if(ABS(c1->body.pos[1]-c2->body.pos[1])<=c1->body.dim[1]/2 && ABS(c1->body.pos[0]-c2->body.pos[0])<distance)
    return 1;

  return 0;
  }*/

/*
static character_t* character_get_close(character_t *c, uint32_t distance)
{
  character_t *char_list,*opponent_close=NULL;
  char_list=character_get_list();
  
  while(char_list)
    {
      if(char_list!=c && character_close_to_each_other(char_list,c, distance))
	{
	  
	  return char_list;
	}
      
      char_list=char_list->next;
    }
  
  return NULL;

  }*/


/*
void character_state_check(character_t *c, uint32_t t)
{
  character_t *opponent_close=NULL;

  switch(c->state)
    {
    case IDLE:
      {
	opponent_close=character_get_close(c, 30000);

	if(opponent_close && opponent_close->life!=0)
	  {
	    c->state=IN_GUARD;
	    c->body.ctrl=CTRL_VEL;
	    return;
	  }
	break;
	
	
      }
      break;
    case IDLE_R:
      {
	opponent_close=character_get_close(c,20000);

	if((c->body.key_pressed & SHIFT) && opponent_close)
	  {
	    c->state=IN_GUARD;
	    c->body.ctrl=CTRL_VEL;
	    return;
	  }
	
	if(opponent_close)
	  {
	    if(opponent_close->state==ATTACK)
	      {
		c->state=HIT;
		c->body.ctrl=CTRL_VEL;
		break;
	      }
	    
	  }
	if(!character_get_close(c,50000))
	  {
	    c->state=IDLE;
	    c->body.ctrl=CTRL_ACC;
	  }
	
      }
      break;
    case IN_GUARD:
      {

	if(c->body.key_pressed & DOWN)
	  {
	    c->state=IDLE_R;
	    c->body.ctrl=CTRL_ACC;
	    return;
	  }
	
	if(c->body.key_pressed & SHIFT)
	  {
	    c->state=ATTACK;
	  }
	
	if(c->body.key_pressed & UP)
	  {
	    c->state=DEFENSE;
	  }

	opponent_close=character_get_close(c,20000);
	
	if(!opponent_close)
	  {
	    c->state=IDLE;
	    c->body.ctrl=CTRL_ACC;
	  }
      }
      break;
      
    case ATTACK:
      if(t-c->body.last_k_t>1000)
	{
	  //c->life--;
	  c->state=IN_GUARD;
	  c->body.last_k_t=t;
	}
      break;
    }
  
}

*/
