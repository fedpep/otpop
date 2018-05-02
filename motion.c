#include <stdint.h>
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





static void motion_apply_external_acc(body_t *b, keyboard_key_t k)
{
  uint32_t i;
  
  if(!IS_ON_A_FLOOR(b->on_constraint))
    return;

  for(i=0;i<4;i++)
    {
      switch(k & (1<<i))
	{
	case UP:
	  if(!(b->last_k & UP))
	    {
	      b->acc[1]+=EXT_ACCEL_Y;
	    }
	  break;
	case DOWN:
	  //b->acc[1]-=EXT_ACCEL;
	  break;
	case RIGHT:    
	  b->acc[0]+=EXT_ACCEL_X;
	  break;
	case LEFT:
	  b->acc[0]-=EXT_ACCEL_X;
	  break;
	}
    }
  
  b->last_k=k;
}

static void motion_apply_gravity(body_t *b)
{
  //b->acc[0]+=0;
  b->acc[1]+=G_ACC;
}

static void motion_apply_friction(body_t *b)
{
  float b_fric=B_FRIC_AIR;
  
  if(IS_ON_A_FLOOR(b->on_constraint))
    b_fric=B_FRIC_FLOOR;

  b->acc[0]+=-b_fric*b->pos_dot[0];
  b->acc[1]+=-b_fric*b->pos_dot[1];
}

static void motion_apply_spring(body_t *b)
{
  b->acc[0]+=-K_HOOK*b->pos[0];
  b->acc[1]+=-K_HOOK*b->pos[1];
}

static void motion_integrate_over_time(body_t *b, uint32_t dt)
{
  /* compute the new speeds */
  b->pos_dot[0]+=b->acc[0]*dt;
  b->pos_dot[1]+=b->acc[1]*dt;

  /* compute the new positions */
  b->pos[0]+=b->pos_dot[0]*dt;
  b->pos[1]+=b->pos_dot[1]*dt;
  
}

static void motion_apply_constraint(body_t *b, uint32_t dt)
{
  int32_t positive_cross,negative_cross;
  float pos_p[2];
  constraint_t* constraint;
  
  b->on_constraint=0;

  constraint=level_get_constraint_list();

  /* compute the previous position: the applied forces may modify 
     the status that we are going to correct with the constraint.
     Thus, we need to evaluate the constraint using the old position */
  pos_p[0]=b->pos[0]-b->pos_dot[0]*dt;
  pos_p[1]=b->pos[1]-b->pos_dot[1]*dt;
  
  //printf("****\ny_prec: %f\n",pos_y_p);
  printf("****\n");
  
  while(constraint)
    {
      if(IS_A_FLOOR(constraint))
	{
	  
	  positive_cross=b->pos[1]<=constraint->p_start[1] && pos_p[1]>=constraint->p_start[1];
      	  negative_cross=b->pos[1]>=constraint->p_start[1] && pos_p[1]<=constraint->p_start[1];

	  if(pos_p[0]>=constraint->p_start[0] && 
	     pos_p[0]<=constraint->p_end[0] && 
	     (positive_cross || negative_cross))
	    {
	      printf("floor constr\n");
	      b->pos[1]=constraint->p_start[1]+positive_cross-negative_cross;
#ifdef ELASTIC_COLLISIONS
	      b->pos_dot[1]=-b->pos_dot[1];
#else
	      b->pos_dot[1]=0;
#endif
	      b->acc[1]=0;
	      b->on_constraint|=ON_A_FLOOR;
	    }
	
	}

      else if(IS_A_WALL(constraint))
	{
	  
	  //printf("--- %d, %d\n",(int)b->pos[0]<=(int)constraint->p_start[0] , (int)pos_p[0]>=(int)constraint->p_start[0]);
	  positive_cross=b->pos[0]<=constraint->p_start[0] && pos_p[0]>=constraint->p_start[0];
      	  negative_cross=b->pos[0]>=constraint->p_start[0] && pos_p[0]<=constraint->p_start[0];
      
	  if(pos_p[1]>=constraint->p_start[1] && 
	     pos_p[1]<=constraint->p_end[1] && 
	     (positive_cross || negative_cross))
	    {
	      printf("wall constr\n");
	      b->pos[0]=constraint->p_start[0]+positive_cross-negative_cross;
#ifdef ELASTIC_COLLISIONS
	      b->pos_dot[0]=-b->pos_dot[0];
	      
#else
	      b->pos_dot[0]=0;
#endif
	      b->acc[0]=0;
	      b->on_constraint|=ON_A_WALL;
	    }
	}

      //PRINTF("x=%d, x_p=%d, y=%d, y_p=%d, p_start[1]=%d,p_end[1]=%d,p_start[0]=%d, pos_cross=%d, neg_cross=%d\n",(int)b->pos[0],(int)pos_p[0],(int)b->pos[1],(int)pos_p[1],(int)constraint->p_start[1],(int)constraint->p_end[1],(int)constraint->p_start[0], positive_cross, negative_cross);

      constraint=constraint->next;
    }
  
  
  
}

void motion_set_pos(body_t *b, int32_t *pos)
{
  b->pos[0]=pos[0];
  b->pos[1]=pos[1];
}

void motion_set_pos_dot(body_t *b, float *pos_dot)
{
  b->pos_dot[0]=pos_dot[0];
  b->pos_dot[1]=pos_dot[1];
}

void motion_init_body(body_t *b)
{
  float z_dot[2]={0,0};
  int32_t z[2]={0,0};

  motion_set_pos(b, z);
  motion_set_pos_dot(b, z_dot);

  b->on_constraint=0;
  b->last_t=SDL_GetTicks();
  b->last_k=0;
}

uint8_t motion_move_body(body_t* b, keyboard_key_t k)
{
  uint32_t dt,t=SDL_GetTicks();

  dt=t-b->last_t;
  if(dt<DT)
    return 0;
  
  b->acc[0]=0;
  b->acc[1]=0;
  
  motion_apply_external_acc(b,k);
  motion_apply_friction(b);
  motion_apply_gravity(b);
  motion_integrate_over_time(b,dt);
  motion_apply_constraint(b,dt);
  
  
  PRINTF("---------------\ndt=%d ms\n",dt);
  PRINTF("position=%d,%d mm\n",b->pos[0],b->pos[1]);
  PRINTF("speed=%.6f,%.6f m/s\n",b->pos_dot[0],b->pos_dot[1]);
  PRINTF("acc=%.3f,%.3f m/s^2\n",b->acc[0],b->acc[1]);

  b->last_t=t;

  return 1;
}


