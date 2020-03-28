#include <stdlib.h>
#include "level.h"
#include "character.h"
#include "graph.h"
#include <stdio.h>

#define ABS(x)  (((x)>=0)?(x):(-(x)))

static character_t *characters_list=NULL;
static character_t *main_character=NULL;

static void character_set_state(character_t *c, chr_state_t new_state)
{
  //printf("character %llx: state change %.2x -> %.2x\n",(unsigned long long)c, c->state, new_state);
  c->state=new_state;
  c->clock=0;
}


static character_t* character_get_close_l(character_t *c, uint32_t distance)
{
  character_t *char_list,*opponent_close=NULL;
  char_list=character_get_list();
  
  while(char_list)
    {
      if(&char_list->body!=&c->body && 
	 motion_body_close_l(&c->body, &char_list->body, distance)) 
	{
	  return char_list;
	}
      
      char_list=char_list->next;
    }
  
  return NULL;

}

static character_t* character_get_close_r(character_t *c, uint32_t distance)
{
  character_t *char_list,*opponent_close=NULL;
  char_list=character_get_list();
  
  while(char_list)
    {
      if(&char_list->body!=&c->body &&
	 motion_body_close_r(&c->body, &char_list->body, distance))
	{
	  return char_list;
	}
      
      char_list=char_list->next;
    }
  
  return NULL;

}

static character_t* character_init_internal(character_kind_t kind)
{
  character_t *c,*aux;
  int32_t m=1;
  int32_t dim[2]={1,1};
  c=(character_t*) malloc(sizeof(character_t));
  
  c->kind=kind;
  c->life=3;
  c->key_pressed=NONE;
  character_set_state(c,CHR_STATE_STAND);
  DIRECTION_SET_RIGHT(&c->body)

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
  c->figure_ptr=graph_init_chr_figure(c->kind);
  
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


void character_state_tick(character_t *c)
{
  character_t *opponent;
  body_t *b = &c->body;
  
  b->acc[0]=0;
  b->acc[1]=0;

  switch(c->state)
    {
    case CHR_STATE_STAND:
      c->clock=0;
      if(!IS_ON_A_FLOOR(b->flags))
	{
	  character_set_state(c, CHR_STATE_FALL);
	  break;
	}
      
      if(IS_HIT(b))
	{
	  character_set_state(c,CHR_STATE_GET_HIT_TO_DEATH);
	  c->life=0;
	  CLEAR_HIT(b);
	}
      else if(c->state==CHR_STATE_STAND && DIRECTION_IS_LEFT(b) && ((c->key_pressed & (UP | LEFT))==(UP | LEFT)))
	{
	  character_set_state(c, CHR_STATE_JUMP_FWD);
	}
      else if(c->state==CHR_STATE_STAND && DIRECTION_IS_RIGHT(b) && ((c->key_pressed & (UP | RIGHT))==(UP | RIGHT)))
	{
	  character_set_state(c, CHR_STATE_JUMP_FWD);
	}
      else if((c->key_pressed & (SHIFT | LEFT)) == (SHIFT | LEFT))
	{
	  character_set_state(c, CHR_STATE_STEP);
	}
      else if((c->key_pressed & (SHIFT | RIGHT)) == (SHIFT | RIGHT))
	{
	  character_set_state(c, CHR_STATE_STEP);
	}
      else if(c->key_pressed & UP)
	{
	  character_set_state(c, CHR_STATE_JUMP);
	}
      else if(c->key_pressed & LEFT)
	{
	  if(DIRECTION_IS_LEFT(b))
	    {
	      character_set_state(c, CHR_STATE_RUN);
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_CHANGE_DIR);
	      
	    }
	}
      else if(c->key_pressed & RIGHT)
	{
	  if(DIRECTION_IS_RIGHT(b))
	    {
	      character_set_state(c, CHR_STATE_RUN);
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_CHANGE_DIR);
	      
	    }

	}
      else if(c->key_pressed & DOWN)
	{
	  if(DIRECTION_IS_RIGHT(b))
	    {
	      if(level_close_to_down_edge_l(b->pos))
		{	  
		  character_set_state(c, CHR_STATE_CLIMB_DOWN);
		  b->suspend_dynamics=1;
		  //b->pos[0]+=1000;
		  b->pos[1]-=12000;
		  //printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  character_set_state(c, CHR_STATE_CROUCH);
		}
	    }
	  else
	    {
	      if(level_close_to_down_edge_r(b->pos))
		{	  
		  character_set_state(c, CHR_STATE_CLIMB_DOWN);
		  b->suspend_dynamics=1;
		  //b->pos[0]-=1000;
		  b->pos[1]-=12000;
		  //printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  character_set_state(c, CHR_STATE_CROUCH);
		}
	      
	    }
	}
      else if(c->key_pressed & CTRL)
	{
	  character_set_state(c, CHR_STATE_FIGHT_UNSHEATHE);
	}
      else if(c->key_pressed & P_BUTTON)
	{
	  character_set_state(c, CHR_STATE_GET_POTION);
	}
      break;

    case CHR_STATE_JUMP:
      if(c->clock==10)
	{
	  //b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==15)
	{
	  if((DIRECTION_IS_LEFT(b) && level_close_to_up_edge_r(b->pos)) || 
	     (DIRECTION_IS_RIGHT(b) && level_close_to_up_edge_l(b->pos)))
	    {
	      //printf("%d %d, move state to climbing up\n",b->pos[0],b->pos[1]);
	      character_set_state(c, CHR_STATE_CLIMB_UP);
	      b->suspend_dynamics=1;
	      break;
	    }
	}
      else if(c->clock==20)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  break;
	}

      c->clock++;
      break;
    case CHR_STATE_RUN:
      if(!IS_ON_A_FLOOR(b->flags))
	{
	  character_set_state(c, CHR_STATE_FALL);
	  break;
	}

      if(((c->key_pressed & RIGHT) && DIRECTION_IS_LEFT(b)) ||
	 ((c->key_pressed & LEFT) && DIRECTION_IS_RIGHT(b)))
	{
	  character_set_state(c, CHR_STATE_RUN_CHANGE_DIR);
	  break;
	}
      else if((c->key_pressed & LEFT) || (c->key_pressed & RIGHT))
	{
	  if(c->key_pressed & LEFT)
	    b->acc[0]-=EXT_ACCEL_X;
	  else
	    b->acc[0]+=EXT_ACCEL_X;

	  if(c->key_pressed & UP)
	    {
	      character_set_state(c, CHR_STATE_RUN_JUMP);
	      break;
	    }
	  if(c->clock>12)
	    c->clock=4;
	}
       else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_CROUCH);
	  break;
	}
       else
	{
	  if(c->clock>0)
	    {
	      character_set_state(c, CHR_STATE_BRAKE);
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND);
	      break;
	    }
	}

       c->clock++;
       if(c->clock>12)
	 c->clock=4;
       
       break;

    case CHR_STATE_BRAKE:
      if(c->clock==7)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  break;
	}
      c->clock++;      
      break;

    case CHR_STATE_RUN_CHANGE_DIR:
      if(c->clock==6)
	{
	  character_set_state(c, CHR_STATE_RUN);
	  DIRECTION_TOGGLE(b);
	  c->clock=3;
	  break;
	}
      c->clock++;
      break;

    case CHR_STATE_CHANGE_DIR:
      if(c->clock==7)
	{
	  DIRECTION_TOGGLE(b);
	  if((c->key_pressed & RIGHT) || (c->key_pressed & LEFT))
	    {
	      character_set_state(c, CHR_STATE_RUN);
	      if(DIRECTION_IS_LEFT(b))
		b->acc[0]+=EXT_ACCEL_X;
	      else
		b->acc[0]-=EXT_ACCEL_X;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND);
	      break;
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_CROUCH:
      if(c->key_pressed & DOWN)
	{
	  if(c->clock!=4)
	    {
	      c->clock++;
	    }
	}
      else
	{
	  
	  if(c->clock==12)
	    {
	      character_set_state(c, CHR_STATE_STAND);
	      break;
	    }
	  c->clock++;
	}
      break;

    case CHR_STATE_RUN_JUMP:
      if(c->clock<8)
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]-=EXT_ACCEL_X;
	  else
	    b->acc[0]+=EXT_ACCEL_X;
	}
      if(c->clock==5)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==10)
	{
	  character_set_state(c, CHR_STATE_RUN);
	}
      c->clock++;
      break;

    
    case CHR_STATE_JUMP_FWD:
      if(c->clock>5 && c->clock<8)
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]-=EXT_ACCEL_X;
	  else
	    b->acc[0]+=EXT_ACCEL_X;
	  
	}
      if(c->clock==7)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==16)
	{
	  if(IS_ON_A_FLOOR(b->flags))
	    {
	      if((c->key_pressed & RIGHT) || (c->key_pressed & LEFT))
		character_set_state(c, CHR_STATE_RUN);
	      else
		character_set_state(c, CHR_STATE_STAND);
	    }
	  else
	    character_set_state(c, CHR_STATE_FALL);

	  c->clock=0;
	  break;
	}
      c->clock++;
      break;
    
    case CHR_STATE_STEP:
      if(c->clock==11)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  break;
	}
      else if(c->clock==4)
	{
	  
	  if((DIRECTION_IS_LEFT(b) && level_close_to_down_edge_l(b->pos)) || 
	     (DIRECTION_IS_RIGHT(b) && level_close_to_down_edge_r(b->pos)))
	    {
	      character_set_state(c, CHR_STATE_STEP_DANG);
	      break;
	    }
	  else
	    {
	      if(DIRECTION_IS_LEFT(b))
		b->acc[0]-=EXT_ACCEL_X;
	      else
		b->acc[0]+=EXT_ACCEL_X;
	    }
	}

      c->clock++;
      break;
    case CHR_STATE_CLIMB_UP:
      if(c->clock==16)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  if(DIRECTION_IS_LEFT(b))
	    b->pos[0]-=1000;
	  else
	    b->pos[0]+=1000;

	  b->pos[1]+=12000;
	  b->suspend_dynamics=0;
	  break;
	}
      c->clock++;
      break;
    
    case CHR_STATE_CLIMB_DOWN:
      if(c->clock==16)
	{
	  if(c->key_pressed & SHIFT)
	    {
	      character_set_state(c, CHR_STATE_HANG);
	      //b->pos[0]+=1000;
	      b->pos[1]+=1000;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND);
	      b->suspend_dynamics=0;
	      break;
	    }
	}
      c->clock++;
      break;
    
    case CHR_STATE_FALL:
      if(IS_ON_A_FLOOR(b->flags))
	{
	  character_set_state(c, CHR_STATE_CROUCH);
	}
      else if(c->clock!=4)
	{
	  c->clock++;
	}
      break;
    
    case CHR_STATE_STEP_DANG:
      character_set_state(c, CHR_STATE_STAND);
      break;
    case CHR_STATE_HANG:
      if(c->clock!=3)
	c->clock++;

      if(!(c->key_pressed & SHIFT))
	{
	  character_set_state(c, CHR_STATE_STAND);
	  b->suspend_dynamics=0;
	  break;
	}
      else if(c->key_pressed & UP)
	{
	  character_set_state(c, CHR_STATE_CLIMB_UP);
	  break;
	}
      break;

    
    case CHR_STATE_FIGHT_UNSHEATHE:
      if(c->clock==3)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_IN_GUARD:
      c->clock=0;
      if(IS_HIT(b))
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]+=EXT_ACCEL_X;
	  else
	    b->acc[0]-=EXT_ACCEL_X;

	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH);
	  CLEAR_HIT(b);
	}
      else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_FIGHT_SHEATHE);
	}
      else if(((c->key_pressed & LEFT) && DIRECTION_IS_LEFT(b)) ||
	      ((c->key_pressed & RIGHT) && DIRECTION_IS_RIGHT(b)))
	{
	  character_set_state(c, CHR_STATE_FIGHT_FWD);
	}
      else if(((c->key_pressed & LEFT) && DIRECTION_IS_RIGHT(b)) ||
	      ((c->key_pressed & RIGHT) && DIRECTION_IS_LEFT(b)))
	{
	  character_set_state(c, CHR_STATE_FIGHT_BACK);
	}
      else if(c->key_pressed & CTRL)
	{
	  character_set_state(c, CHR_STATE_FIGHT_ATTACK);
	}
      break;
    case CHR_STATE_FIGHT_SHEATHE:
      if(c->clock==7)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_FWD:
      if(c->clock==0)
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]-=EXT_ACCEL_X;
	  else
	    b->acc[0]+=EXT_ACCEL_X;
	}
      else if(c->clock==4)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_BACK:
      if(c->clock==0)
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]+=EXT_ACCEL_X;
	  else
	    b->acc[0]-=EXT_ACCEL_X;
	}
      else if(c->clock==4)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_ATTACK:
      if(IS_HIT(b))
	{
	  if(DIRECTION_IS_LEFT(b))
	    b->acc[0]+=EXT_ACCEL_X;
	  else
	    b->acc[0]-=EXT_ACCEL_X;

	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH);
	  CLEAR_HIT(b);
	  break;
	}

      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD);
	  break;
	}
      else if(c->clock==1)
	{
	  if(DIRECTION_IS_LEFT(b))
	    opponent=character_get_close_l(c, HIT_DISTANCE);
	  else
	    opponent=character_get_close_r(c, HIT_DISTANCE);

	  if(opponent)
	    {
	      SET_HIT(&opponent->body);
	      //printf("opponent close: %x\n",(uint32_t)opponent);
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_GET_POTION:
      if(c->clock==14)
	{
	  character_set_state(c, CHR_STATE_STAND);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT:
      if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT_TO_DEATH:
      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_DEAD);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_DEAD:
      c->clock=0;
      break;
    }

  //printf("character %llx: life %d, flags %.2x, state %.2x, clock: %d\n",(unsigned long long)c, c->life, b->flags,c->state,c->clock);

}


void character_set_initial_state(character_t *c, uint32_t *pos, float *vel, direction_t dir)
{
  uint8_t i;
  
  for(i=0;i<2;i++)
    {
      c->body.pos[i]=pos[i];
      c->body.vel[i]=vel[i];
    }

  character_set_state(c,CHR_STATE_STAND);

  if(dir==DIR_LEFT)
    {
      DIRECTION_SET_LEFT(&c->body);
    }
  else
    {
      DIRECTION_SET_RIGHT(&c->body);
    }


}

