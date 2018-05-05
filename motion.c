#include <stdint.h>
#include "keyboard.h"
#include "motion.h"
#include "level.h"
#include "character.h"

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif


#define ABS(x)  (((x)>=0)?(x):(-(x)))


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
	  b->acc[1]+=EXT_ACCEL_Y;
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

  b->acc[0]+=-b_fric*b->vel[0]/b->mass;
  b->acc[1]+=-b_fric*b->vel[1]/b->mass;
}

static void motion_apply_spring(body_t *b)
{
  b->acc[0]+=-K_HOOK*b->pos[0]/b->mass;
  b->acc[1]+=-K_HOOK*b->pos[1]/b->mass;
}

static void motion_integrate_over_time(body_t *b, uint32_t dt)
{
  /* compute the new speeds */
  b->vel[0]+=b->acc[0]*dt;
  b->vel[1]+=b->acc[1]*dt;

  /* compute the new positions */
  b->pos[0]+=b->vel[0]*dt;
  b->pos[1]+=b->vel[1]*dt;
  
}

static void motion_apply_constraints(body_t *b, uint32_t dt)
{
  int32_t positive_cross,negative_cross;
  float pos_p[2];
  constraint_t* constraint;
  
  b->on_constraint=0;

  constraint=level_get_constraint_list();

  /* compute the previous position: the applied forces may modify 
     the status that we are going to correct with the constraint.
     Thus, we need to evaluate the constraint using the old position */
  pos_p[0]=b->pos[0]-b->vel[0]*dt;
  pos_p[1]=b->pos[1]-b->vel[1]*dt;
  
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
	      b->vel[1]=-b->vel[1];
#else
	      b->vel[1]=0;
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
	      b->vel[0]=-b->vel[0];
	      
#else
	      b->vel[0]=0;
#endif
	      b->acc[0]=0;
	      b->on_constraint|=ON_A_WALL;
	    }
	}

      //PRINTF("x=%d, x_p=%d, y=%d, y_p=%d, p_start[1]=%d,p_end[1]=%d,p_start[0]=%d, pos_cross=%d, neg_cross=%d\n",(int)b->pos[0],(int)pos_p[0],(int)b->pos[1],(int)pos_p[1],(int)constraint->p_start[1],(int)constraint->p_end[1],(int)constraint->p_start[0], positive_cross, negative_cross);

      constraint=constraint->next;
    }
  
  
  
}

static void motion_apply_characters(body_t *b, uint32_t dt)
{
  character_t *c;
  body_t *cb;
  float aux;
  float pos_p[2];
  int32_t positive_cross,negative_cross;
  
  c=character_get_list();

  /* compute the previous position to understand if we are crossing the other characters */
  pos_p[0]=b->pos[0]-b->vel[0]*dt;
  pos_p[1]=b->pos[1]-b->vel[1]*dt;
  
  while(c)
    {
      cb=&c->body;
      
      /* we want to avoid to do the computation against ourself... move on in this case! */
      if(cb!=b)
	{	  
	  positive_cross=b->pos[0]<=cb->pos[0] && pos_p[0]>=cb->pos[0] && ABS(b->pos[1]-cb->pos[1])<=cb->dim[1]/2;// TBF: that number (60) should be dim[1]
	  negative_cross=b->pos[0]>=cb->pos[0] && pos_p[0]<=cb->pos[0] && ABS(b->pos[1]-cb->pos[1])<=cb->dim[1]/2;
  
	  //printf("char motion: pos:%d, neg:%d, mass=%d\n",positive_cross, negative_cross, cb->mass);

	  if(negative_cross || positive_cross)
	    {
	      aux=b->vel[0];
	      b->vel[0]=((b->mass-cb->mass)*b->vel[0]+2*cb->mass*cb->vel[0])/(b->mass+cb->mass);
	      cb->vel[0]=((cb->mass-b->mass)*cb->vel[0]+2*b->mass*aux)/(b->mass+cb->mass);
	      
	      /* we exchange the positions: this is more a workaround than other...and it works as the masses are almost the same
	       we should mark the other body as hit with a certain velocity and let itself to adjust its own velocity and position */
	      aux=b->pos[0];
	      b->pos[0]=cb->pos[0];
	      cb->pos[0]=aux;
	      
	    }
	}

      c=c->next;
    }
}

void motion_set_pos(body_t *b, int32_t *pos)
{
  b->pos[0]=pos[0];
  b->pos[1]=pos[1];
}

void motion_set_vel(body_t *b, float *vel)
{
  b->vel[0]=vel[0];
  b->vel[1]=vel[1];
}

void motion_set_dim(body_t *b, int32_t *dim)
{
  b->dim[0]=dim[0];
  b->dim[1]=dim[1];
}

void motion_init_body(body_t *b, int32_t *dim, int32_t mass)
{
  float z_dot[2]={0,0};
  int32_t z[2]={0,0};

  motion_set_pos(b, z);
  motion_set_vel(b, z_dot);
  motion_set_dim(b, dim);
  
  b->mass=mass;
  b->on_constraint=0;
  b->last_t=SDL_GetTicks();
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
  motion_apply_constraints(b,dt);
  motion_apply_characters(b,dt);
  
  PRINTF("-----%lx-main=%d----\ndt=%d ms\n",(long unsigned int)b,b==&(character_get_main()->body),dt);
  PRINTF("position=%d,%d mm\n",b->pos[0],b->pos[1]);
  PRINTF("velocity=%.6f,%.6f m/s\n",b->vel[0],b->vel[1]);
  PRINTF("acc=%.3f,%.3f m/s^2\n",b->acc[0],b->acc[1]);

  b->last_t=t;

  return 1;
}


