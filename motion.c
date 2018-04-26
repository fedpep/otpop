#include <stdint.h>
#include "character.h"
#include "keyboard.h"
#include "motion.h"
#include "level.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif

static keyboard_key_t k_prec=0;
static uint32_t last_t=0;


static void motion_apply_external_acc(float* acc, uint8_t on_a_constraint, keyboard_key_t k)
{
  uint32_t i;
  
  if(!IS_ON_A_FLOOR(on_a_constraint))
    return;

  for(i=0;i<4;i++)
    {
      switch(k & (1<<i))
	{
	case UP:
	  if(!(k_prec & UP))
	    {
	      acc[1]+=EXT_ACCEL_Y;
	    }
	  break;
	case DOWN:
	  //acc[1]-=EXT_ACCEL;
	  break;
	case RIGHT:    
	  acc[0]+=EXT_ACCEL_X;
	  break;
	case LEFT:
	  acc[0]-=EXT_ACCEL_X;
	  break;
	}
    }
  
  k_prec=k;
}

static void motion_apply_gravity(float *acc)
{
  //acc[0]+=0;
  acc[1]+=G_ACC;
}

static void motion_apply_friction(float *acc, float *vel, uint8_t on_a_constraint)
{
  float b=B_FRIC_AIR;
  
  if(IS_ON_A_FLOOR(on_a_constraint))
    b=B_FRIC_FLOOR;

  acc[0]+=-b*vel[0];
  acc[1]+=-b*vel[1];
}

static void motion_apply_spring(float *acc, float *pos)
{
  acc[0]+=-K_HOOK*pos[0];
  acc[1]+=-K_HOOK*pos[1];
}

static void integrate_over_time(float *state, float *input)
{
  state[0]+=input[0]*DT;
  state[1]+=input[1]*DT;
}

static uint8_t motion_apply_constraint(float *pos, float *vel, float *acc)
{
  int32_t positive_cross,negative_cross;
  float pos_p[2];
  constraint_t* constraint;
  uint8_t on_a_constraint=0;

  constraint=level_get_constraint_list();

  /* compute the previous position: the applied forces may modify 
     the status that we are going to correct with the constraint.
     Thus, we need to evaluate the constraint using the old position */
  pos_p[0]=pos[0]-vel[0]*DT;
  pos_p[1]=pos[1]-vel[1]*DT;
  


  //printf("****\ny_prec: %f\n",pos_y_p);
  printf("****\n");
  
  while(constraint)
    {
      if(IS_A_FLOOR(constraint))
	{
	  
	  positive_cross=(int)pos[1]<=(int)constraint->p_start[1] && (int)pos_p[1]>=(int)constraint->p_start[1];
      	  negative_cross=(int)pos[1]>=(int)constraint->p_start[1] && (int)pos_p[1]<=(int)constraint->p_start[1];

	  if(pos_p[0]>=constraint->p_start[0] && 
	     pos_p[0]<=constraint->p_end[0] && 
	     (positive_cross || negative_cross))
	    {
	      printf("floor constr\n");
	      pos[1]=constraint->p_start[1]+positive_cross-negative_cross;
#ifdef ELASTIC_COLLISIONS
	      vel[1]=-vel[1];
#else
	      vel[1]=0;
#endif
	      acc[1]=0;
	      on_a_constraint|=ON_A_FLOOR;
	    }
	
	}

      else if(IS_A_WALL(constraint))
	{
	  
	  //printf("--- %d, %d\n",(int)pos[0]<=(int)constraint->p_start[0] , (int)pos_p[0]>=(int)constraint->p_start[0]);
	  positive_cross=(int)pos[0]<=(int)constraint->p_start[0] && (int)pos_p[0]>=(int)constraint->p_start[0];
      	  negative_cross=(int)pos[0]>=(int)constraint->p_start[0] && (int)pos_p[0]<=(int)constraint->p_start[0];
      
	  if((int)pos_p[1]>=(int)constraint->p_start[1] && 
	     (int)pos_p[1]<=(int)constraint->p_end[1] && 
	     (positive_cross || negative_cross))
	    {
	      printf("wall constr\n");
	      pos[0]=constraint->p_start[0]+positive_cross-negative_cross;
#ifdef ELASTIC_COLLISIONS
	      vel[0]=-vel[0];
#else
	      vel[0]=0;
#endif
	      acc[0]=0;
	      on_a_constraint|=ON_A_WALL;
	    }
	}

      //printf("x=%d, x_p=%d, y=%d, y_p=%d, p_start[1]=%d,p_end[1]=%d,p_start[0]=%d\n",(int)pos[0],(int)pos_p[0],(int)pos[1],(int)pos_p[1],(int)constraint->p_start[1],(int)constraint->p_end[1],(int)constraint->p_start[0]);

      constraint=constraint->next;
    }
  
  return on_a_constraint;
}

void motion_move_character(character_t* c, keyboard_key_t k)
{
  uint32_t dt,t=SDL_GetTicks();

  dt=t-last_t;
  if(dt<DT)
    return;
  
  c->acc[0]=0;
  c->acc[1]=0;
    
  motion_apply_friction(c->acc,c->pos_dot,c->on_constraint);
  motion_apply_external_acc(c->acc,c->on_constraint,k);
  motion_apply_gravity(c->acc);
  integrate_over_time(c->pos_dot, c->acc);
  integrate_over_time(c->pos, c->pos_dot);
  c->on_constraint=motion_apply_constraint(c->pos, c->pos_dot, c->acc);
  
  
  PRINTF("---------------\ndt=%d ms\n",DT);
  PRINTF("position=%.3f,%.3f mm\n",c->pos[0],c->pos[1]);
  PRINTF("speed=%.6f,%.6f m/s\n",c->pos_dot[0],c->pos_dot[1]);
  PRINTF("acc=%.3f,%.3f m/s^2\n",c->acc[0],c->acc[1]);

  last_t=t;
}


