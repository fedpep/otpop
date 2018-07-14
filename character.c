#include <stdlib.h>
#include "level.h"
#include "character.h"
#include <stdio.h>

#define ABS(x)  (((x)>=0)?(x):(-(x)))

static character_t *characters_list=NULL;
static character_t *main_character=NULL;

static void character_set_state(character_t *c, chr_state_t new_state)
{
  //printf("character %llx: state change %.2x ->%.2x\n",(unsigned long long)c, c->state, new_state);
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
  character_set_state(c,CHR_STATE_STAND_R);

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


void character_state_tick(character_t *c)
{
  character_t *opponent;
  body_t *b = &c->body;
  
  b->acc[0]=0;
  b->acc[1]=0;

  switch(c->state)
    {
    case CHR_STATE_STAND_L:
    case CHR_STATE_STAND_R:
      c->clock=0;
      if(!IS_ON_A_FLOOR(b->event))
	{
	  character_set_state(c, (c->state==CHR_STATE_STAND_L)?(CHR_STATE_FALL_L):(CHR_STATE_FALL_R));
	  break;
	}
      
      if(IS_HIT(b))
	{
	  character_set_state(c,(c->state==CHR_STATE_STAND_L)?(CHR_STATE_GET_HIT_TO_DEATH_L):(CHR_STATE_GET_HIT_TO_DEATH_R));
	  c->life=0;
	  CLEAR_HIT(b);
	}
      else if(c->state==CHR_STATE_STAND_L && ((c->key_pressed & (UP | LEFT))==(UP | LEFT)))
	{
	  character_set_state(c, CHR_STATE_JUMP_FWD_L);
	}
      else if(c->state==CHR_STATE_STAND_R && ((c->key_pressed & (UP | RIGHT))==(UP | RIGHT)))
	{
	  character_set_state(c, CHR_STATE_JUMP_FWD_R);
	}
      else if((c->key_pressed & (SHIFT | LEFT)) == (SHIFT | LEFT))
	{
	  character_set_state(c, CHR_STATE_STEP_L);
	}
      else if((c->key_pressed & (SHIFT | RIGHT)) == (SHIFT | RIGHT))
	{
	  character_set_state(c, CHR_STATE_STEP_R);
	}
      else if(c->key_pressed & UP)
	{
	  if(c->state==CHR_STATE_STAND_L)
	    {
	      character_set_state(c, CHR_STATE_JUMP_L);
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_JUMP_R);
	    }
	}
      else if(c->key_pressed & LEFT)
	{
	  if(c->state==CHR_STATE_STAND_L)
	    {
	      character_set_state(c, CHR_STATE_RUN_L);
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_CHANGE_DIR_R2L);
	    }
	}
      else if(c->key_pressed & RIGHT)
	{
	  if(c->state==CHR_STATE_STAND_R)
	    {
	      character_set_state(c, CHR_STATE_RUN_R);
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_CHANGE_DIR_L2R);
	    }

	}
      else if(c->key_pressed & DOWN)
	{
	  if(c->state==CHR_STATE_STAND_L)
	    {
	      if(level_close_to_down_edge_l(b->pos))
		{	  
		  character_set_state(c, CHR_STATE_CLIMB_DOWN_L);
		  b->suspend_dynamics=1;
		  //b->pos[0]+=1000;
		  b->pos[1]-=12000;
		  printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  character_set_state(c, CHR_STATE_CROUCH_L);
		}
	    }
	  else
	    {
	      if(level_close_to_down_edge_r(b->pos))
		{	  
		  character_set_state(c, CHR_STATE_CLIMB_DOWN_R);
		  b->suspend_dynamics=1;
		  //b->pos[0]-=1000;
		  b->pos[1]-=12000;
		  printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  character_set_state(c, CHR_STATE_CROUCH_R);
		}
	      
	    }
	}
      else if(c->key_pressed & CTRL)
	{
	  character_set_state(c, (c->state==CHR_STATE_STAND_L)?(CHR_STATE_FIGHT_UNSHEATHE_L):(CHR_STATE_FIGHT_UNSHEATHE_R));
	}
      else if(c->key_pressed & P_BUTTON)
	{
	  character_set_state(c, (c->state==CHR_STATE_STAND_L)?(CHR_STATE_GET_POTION_L):(CHR_STATE_GET_POTION_R));
	}
      break;

    case CHR_STATE_JUMP_L:
    case CHR_STATE_JUMP_R:
      if(c->clock==10)
	{
	  //b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==15)
	{
	  if((c->state==CHR_STATE_JUMP_L && level_close_to_up_edge_l(b->pos)) || (c->state==CHR_STATE_JUMP_R && level_close_to_up_edge_r(b->pos)))
	    {
	      printf("%d %d, move state to climbing up\n",b->pos[0],b->pos[1]);
	      character_set_state(c, (c->state==CHR_STATE_JUMP_L)?(CHR_STATE_CLIMB_UP_L):(CHR_STATE_CLIMB_UP_R));
	      b->suspend_dynamics=1;
	      break;
	    }
	}
      else if(c->clock==20)
	{
	  character_set_state(c, (c->state==CHR_STATE_JUMP_L)?(CHR_STATE_STAND_L):(CHR_STATE_STAND_R));
	  break;
	}

      c->clock++;
      break;
    case CHR_STATE_RUN_L:
      if(!IS_ON_A_FLOOR(b->event))
	{
	  character_set_state(c, CHR_STATE_FALL_L);
	  break;
	}

       if(c->key_pressed & RIGHT)
	{
	  character_set_state(c, CHR_STATE_INVERT_L2R);
	  break;
	}
       else if(c->key_pressed & LEFT)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	  if(c->key_pressed & UP)
	    {
	      character_set_state(c, CHR_STATE_RUN_JUMP_L);
	      break;
	    }
	  if(c->clock>12)
	    c->clock=4;
	}
       else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_CROUCH_L);
	  break;
	}
      else
	{
	  if(c->clock>0)
	    {
	      character_set_state(c, CHR_STATE_BRAKE_L);
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_L);
	      break;
	    }
	}

       c->clock++;
       if(c->clock>12)
	 c->clock=4;

       break;
    case CHR_STATE_RUN_R:
      if(!IS_ON_A_FLOOR(b->event))
	{
	  character_set_state(c, CHR_STATE_FALL_R);
	  break;
	}
      if(c->key_pressed & LEFT)
	{
	  character_set_state(c, CHR_STATE_INVERT_R2L);
	}
      else if(c->key_pressed & RIGHT)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	  if(c->key_pressed & UP)
	    {
	      character_set_state(c, CHR_STATE_RUN_JUMP_R);
	    }

	  
	}
      else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_CROUCH_R);
	  break;
	}
      else
	{
	  if(c->clock>0)
	    {
	      character_set_state(c, CHR_STATE_BRAKE_R);
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_R);
	    }
	}

      c->clock++;
      if(c->clock>12)
	c->clock=4;
      break;
    case CHR_STATE_BRAKE_L:
    case CHR_STATE_BRAKE_R:
      if(c->clock==7)
	{
	  if(c->state==CHR_STATE_BRAKE_L)
	    {
	      character_set_state(c, CHR_STATE_STAND_L);
	      break;
	      /*if(c->key_pressed & RIGHT)
		{
		  c->state=CHR_STATE_INVERT_L2R;
		}
		c->clock=0;*/
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_R);
	      break;
	      /*if(c->key_pressed & LEFT)
		{
		  c->state=CHR_STATE_INVERT_R2L;
		}
		c->clock=0;*/
	    }
	}
      c->clock++;      
      break;

    case CHR_STATE_INVERT_L2R:
      if(c->clock==8)
	{
	  character_set_state(c, CHR_STATE_RUN_R);
	  c->clock=3;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_INVERT_R2L:
      if(c->clock==8)
	{
	  character_set_state(c, CHR_STATE_RUN_L);
	  c->clock=3;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_CHANGE_DIR_L2R:
      if(c->clock==7)
	{
	  if(c->key_pressed & RIGHT)
	    {
	      character_set_state(c, CHR_STATE_RUN_R);
	      b->acc[0]+=EXT_ACCEL_X;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_R);
	      break;
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_CHANGE_DIR_R2L:
      if(c->clock==7)
	{
	  if(c->key_pressed & LEFT)
	    {
	      character_set_state(c, CHR_STATE_RUN_L);
	      b->acc[0]-=EXT_ACCEL_X;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_L);
	      break;
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_CROUCH_L:
    case CHR_STATE_CROUCH_R:
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
	      character_set_state(c, (c->state==CHR_STATE_CROUCH_L)?CHR_STATE_STAND_L:CHR_STATE_STAND_R);
	      break;
	    }
	  c->clock++;
	}
      break;

    case CHR_STATE_RUN_JUMP_L:
      if(c->clock<8)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      if(c->clock==5)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==10)
	{
	  character_set_state(c, CHR_STATE_RUN_L);
	}
      c->clock++;
      break;

    case CHR_STATE_RUN_JUMP_R:
      if(c->clock<8)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      if(c->clock==5)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==10)
	{
	  character_set_state(c, CHR_STATE_RUN_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_JUMP_FWD_L:
      if(c->clock>5 && c->clock<8)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      if(c->clock==7)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==16)
	{
	  if(IS_ON_A_FLOOR(b->event))
	    {
	      if(c->key_pressed & LEFT)
		character_set_state(c, CHR_STATE_RUN_L);
	      else
		character_set_state(c, CHR_STATE_STAND_L);
	    }
	  else
	    character_set_state(c, CHR_STATE_FALL_L);

	  c->clock=0;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_JUMP_FWD_R:
      if(c->clock>5 && c->clock<8)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      if(c->clock==7)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(c->clock==16)
	{
	  if(IS_ON_A_FLOOR(b->event))
	    {
	      if(c->key_pressed & RIGHT)
		character_set_state(c, CHR_STATE_RUN_R);
	      else
		character_set_state(c, CHR_STATE_STAND_R);
	    }
	  else
	    character_set_state(c, CHR_STATE_FALL_R);

	  c->clock=0;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_STEP_L:
      if(c->clock==11)
	{
	  character_set_state(c, CHR_STATE_STAND_L);
	  break;
	}
      else if(c->clock==4)
	{
	  
	  if(level_close_to_down_edge_r(b->pos))
	    {
	      character_set_state(c, CHR_STATE_STEP_DANG_L);
	      break;
	    }
	  else
	    {
	      b->acc[0]-=EXT_ACCEL_X;
	    }
	}

      c->clock++;
      break;
    case CHR_STATE_STEP_R:
      if(c->clock==11)
	{
	  character_set_state(c, CHR_STATE_STAND_R);
	  break;
	}
      else if(c->clock==4)
	{
	  if(level_close_to_down_edge_l(b->pos))
	    {
	      character_set_state(c, CHR_STATE_STEP_DANG_R);
	      break;
	    }
	  else
	    {
	      b->acc[0]+=EXT_ACCEL_X;
	    }
	}

      c->clock++;
      break;
    case CHR_STATE_CLIMB_UP_L:   
      if(c->clock==16)
	{
	  character_set_state(c, CHR_STATE_STAND_L);
	  b->pos[0]-=1000;
	  b->pos[1]+=12000;
	  b->suspend_dynamics=0;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_CLIMB_UP_R:
      if(c->clock==16)
	{
	  character_set_state(c, CHR_STATE_STAND_R);
	  b->pos[0]+=1000;
	  b->pos[1]+=12000;
	  b->suspend_dynamics=0;
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_CLIMB_DOWN_L:   
      if(c->clock==16)
	{
	  if(c->key_pressed & SHIFT)
	    {
	      character_set_state(c, CHR_STATE_HANG_L);
	      //b->pos[0]+=1000;
	      b->pos[1]+=1000;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_L);
	      b->suspend_dynamics=0;
	      break;
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_CLIMB_DOWN_R:   
      if(c->clock==16)
	{
	  if(c->key_pressed & SHIFT)
	    {
	      character_set_state(c, CHR_STATE_HANG_R);
	      //b->pos[0]-=1000;
	      b->pos[1]+=1000;
	      break;
	    }
	  else
	    {
	      character_set_state(c, CHR_STATE_STAND_R);
	      b->suspend_dynamics=0;
	      break;
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_FALL_L:
      if(IS_ON_A_FLOOR(b->event))
	{
	  character_set_state(c, CHR_STATE_CROUCH_L);
	}
      else if(c->clock!=4)
	{
	  c->clock++;
	}
      break;
    case CHR_STATE_FALL_R:
      if(IS_ON_A_FLOOR(b->event))
	{
	  character_set_state(c, CHR_STATE_CROUCH_R);
	}
      else if(c->clock!=4)
	{
	  c->clock++;
	}
      break;
    case CHR_STATE_STEP_DANG_L:
      character_set_state(c, CHR_STATE_STAND_L);
      break;
    case CHR_STATE_STEP_DANG_R:
      character_set_state(c, CHR_STATE_STAND_R);
      break;
    case CHR_STATE_HANG_L:
      if(c->clock!=3)
	c->clock++;

      if(!(c->key_pressed & SHIFT))
	{
	  character_set_state(c, CHR_STATE_STAND_L);
	  b->suspend_dynamics=0;
	  break;
	}
      else if(c->key_pressed & UP)
	{
	  character_set_state(c, CHR_STATE_CLIMB_UP_L);
	  break;
	}
      break;

    case CHR_STATE_HANG_R:
      if(c->clock!=3)
	c->clock++;

      if(!(c->key_pressed & SHIFT))
	{
	  character_set_state(c, CHR_STATE_STAND_R);
	  b->suspend_dynamics=0;
	  break;
	}
      else if(c->key_pressed & UP)
	{
	  character_set_state(c, CHR_STATE_CLIMB_UP_R);
	  break;
	}
      break;
    case CHR_STATE_FIGHT_UNSHEATHE_L:
      if(c->clock==3)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_L);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_UNSHEATHE_R:
      if(c->clock==3)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_IN_GUARD_L:
      c->clock=0;
      if(IS_HIT(b))
	{
	  b->acc[0]+=EXT_ACCEL_X;
	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT_L);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH_L);
	  CLEAR_HIT(b);
	}
      else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_FIGHT_SHEATHE_L);
	}
      else if(c->key_pressed & LEFT)
	{
	  character_set_state(c, CHR_STATE_FIGHT_FWD_L);
	}
      else if(c->key_pressed & RIGHT)
	{
	  character_set_state(c, CHR_STATE_FIGHT_BACK_L);
	}
      else if(c->key_pressed & CTRL)
	{
	  character_set_state(c, CHR_STATE_FIGHT_ATTACK_L);
	}
      break;
    case CHR_STATE_FIGHT_IN_GUARD_R:
      c->clock=0;
      if(IS_HIT(b))
	{
	  b->acc[0]-=EXT_ACCEL_X;
	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT_R);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH_R);
	  CLEAR_HIT(b);
	}
      else if(c->key_pressed & DOWN)
	{
	  character_set_state(c, CHR_STATE_FIGHT_SHEATHE_R);
	}
      else if(c->key_pressed & RIGHT)
	{
	  character_set_state(c, CHR_STATE_FIGHT_FWD_R);
	}
      else if(c->key_pressed & LEFT)
	{
	  character_set_state(c, CHR_STATE_FIGHT_BACK_R);
	}
      else if(c->key_pressed & CTRL)
	{
	  character_set_state(c, CHR_STATE_FIGHT_ATTACK_R);
	}
      break;
    case CHR_STATE_FIGHT_SHEATHE_L:
      if(c->clock==7)
	{
	  character_set_state(c, CHR_STATE_STAND_L);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_SHEATHE_R:
      if(c->clock==7)
	{
	  character_set_state(c, CHR_STATE_STAND_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_FWD_L:
      if(c->clock==0)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      else if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_L);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_FWD_R:
      if(c->clock==0)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      else if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_R);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_BACK_L:
      if(c->clock==0)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      else if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_L);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_BACK_R:
      if(c->clock==0)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      else if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_R);
	  break;
	}
       
      c->clock++;
      break;
    case CHR_STATE_FIGHT_ATTACK_L:
      if(IS_HIT(b))
	{
	  b->acc[0]+=EXT_ACCEL_X;
	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT_L);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH_L);
	  CLEAR_HIT(b);
	}

      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_L);
	  break;
	}
      else if(c->clock==2)
	{
	  opponent=character_get_close_l(c, 8000);
	  if(opponent)
	    {
	      SET_HIT(&opponent->body);
	      //printf("opponent close: %x\n",(uint32_t)opponent);
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_FIGHT_ATTACK_R:
      if(IS_HIT(b))
	{
	  b->acc[0]-=EXT_ACCEL_X;
	  c->life--;
	  if(c->life!=0)
	    character_set_state(c, CHR_STATE_GET_HIT_R);
	  else
	    character_set_state(c, CHR_STATE_GET_HIT_TO_DEATH_R);
	  CLEAR_HIT(b);
	}

      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_R);
	  break;
	}
      else if(c->clock==2)
	{
	  opponent=character_get_close_r(c, 8000);
	  if(opponent)
	    {
	      SET_HIT(&opponent->body);
	      //printf("opponent close: %x\n",(uint32_t)opponent);
	    }
	}
      c->clock++;
      break;
    case CHR_STATE_GET_POTION_L:
      if(c->clock==14)
	{
	  character_set_state(c, CHR_STATE_STAND_L);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_POTION_R:
      if(c->clock==14)
	{
	  character_set_state(c, CHR_STATE_STAND_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT_L:
      if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_L);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT_R:
      if(c->clock==2)
	{
	  character_set_state(c, CHR_STATE_FIGHT_IN_GUARD_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT_TO_DEATH_L:
      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_DEAD_L);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_GET_HIT_TO_DEATH_R:
      if(c->clock==5)
	{
	  character_set_state(c, CHR_STATE_DEAD_R);
	  break;
	}
      c->clock++;
      break;
    case CHR_STATE_DEAD_L:
    case CHR_STATE_DEAD_R:
      c->clock=0;
      break;
    }

  //printf("character %llx: life %d, event %.2x, state %.2x, clock: %d\n",(unsigned long long)c, c->life, b->event,c->state,c->clock);

}


void character_set_initial_state(character_t *c, uint32_t *pos, float *vel, direction_t dir)
{
  uint8_t i;
  
  for(i=0;i<2;i++)
    {
      c->body.pos[i]=pos[i];
      c->body.vel[i]=vel[i];
    }

  character_set_state(c,(dir==DIR_LEFT)?(CHR_STATE_STAND_L):(CHR_STATE_STAND_R));

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
