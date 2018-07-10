#include <stdint.h>
#include "keyboard.h"
#include "motion.h"
#include "level.h"
#include "character.h"

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif


#define ABS(x)  (((x)>=0)?(x):(-(x)))


static void motion_apply_external_acc(body_t *b,uint32_t t)
{
  
  
  printf("on_constraint: %.2x, state %.2x, clock: %d\n",b->on_constraint,b->state,b->clock);


  switch(b->state)
    {
    case MOTION_STATE_STAND_L:
    case MOTION_STATE_STAND_R:
      b->clock=0;
      if(!IS_ON_A_FLOOR(b->on_constraint))
	{
	  b->state=(b->state==MOTION_STATE_STAND_L)?(MOTION_STATE_FALL_L):(MOTION_STATE_FALL_R);
	  break;
	}
      

      if(b->state==MOTION_STATE_STAND_L && ((b->key_pressed & (UP | LEFT))==(UP | LEFT)))
	{
	  b->state=MOTION_STATE_JUMP_FWD_L;
	}
      else if(b->state==MOTION_STATE_STAND_R && ((b->key_pressed & (UP | RIGHT))==(UP | RIGHT)))
	{
	  b->state=MOTION_STATE_JUMP_FWD_R;
	}
      else if(b->key_pressed & SHIFT)
	{
	  if(b->key_pressed & LEFT)
	    {
	      b->state=MOTION_STATE_STEP_L;
	    }
	  else if(b->key_pressed & RIGHT)
	    {
	      b->state=MOTION_STATE_STEP_R;
	    }
	  b->clock=0;
	}
      else if(b->key_pressed & UP)
	{
	  if(b->state==MOTION_STATE_STAND_L)
	    {
	      b->state=MOTION_STATE_JUMP_L;
	    }
	  else
	    {
	      b->state=MOTION_STATE_JUMP_R;
	    }
	}
      else if(b->key_pressed & LEFT)
	{
	  if(b->state==MOTION_STATE_STAND_L)
	    {
	      b->state=MOTION_STATE_RUN_L;
	    }
	  else
	    {
	      b->state=MOTION_STATE_CHANGE_DIR_R2L;
	    }
	}
      else if(b->key_pressed & RIGHT)
	{
	  if(b->state==MOTION_STATE_STAND_R)
	    {
	      b->state=MOTION_STATE_RUN_R;
	    }
	  else
	    {
	      b->state=MOTION_STATE_CHANGE_DIR_L2R;
	    }

	}
      else if(b->key_pressed & DOWN)
	{
	  if(b->state==MOTION_STATE_STAND_L)
	    {
	      if(level_close_to_down_edge_l(b->pos))
		{	  
		  b->state=MOTION_STATE_CLIMB_DOWN_L;
		  b->suspend_dynamics=1;
		  b->pos[0]+=1000;
		  b->pos[1]-=12000;
		  printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  b->state=MOTION_STATE_CROUCH_L;
		}
	    }
	  else
	    {
	      if(level_close_to_down_edge_r(b->pos))
		{	  
		  b->state=MOTION_STATE_CLIMB_DOWN_R;
		  b->suspend_dynamics=1;
		  b->pos[0]-=1000;
		  b->pos[1]-=12000;
		  printf("%d %d, move state to climbing down\n",b->pos[0],b->pos[1]);
		}
	      else
		{
		  b->state=MOTION_STATE_CROUCH_R;
		}
	      
	    }
	}
      
      break;

    case MOTION_STATE_JUMP_L:
    case MOTION_STATE_JUMP_R:
      if(b->clock==10)
	{
	  //b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(b->clock==15)
	{
	  if((b->state==MOTION_STATE_JUMP_L && level_close_to_up_edge_l(b->pos)) || (b->state==MOTION_STATE_JUMP_R && level_close_to_up_edge_r(b->pos)))
	    {
	      printf("%d %d, move state to climbing up\n",b->pos[0],b->pos[1]);
	      b->state=(b->state==MOTION_STATE_JUMP_L)?(MOTION_STATE_CLIMB_UP_L):(MOTION_STATE_CLIMB_UP_R);
	      b->clock=0;
	      b->suspend_dynamics=1;
	    }
	}
      else if(b->clock==20)
	{
	  b->state=(b->state==MOTION_STATE_JUMP_L)?(MOTION_STATE_STAND_L):(MOTION_STATE_STAND_R);
	}
      b->clock++;
      b->last_k_t=t;//boh
      break;
    case MOTION_STATE_RUN_L:
      if(!IS_ON_A_FLOOR(b->on_constraint))
	{
	  b->state=MOTION_STATE_FALL_L;
	  b->clock=0;
	  break;
	}

       if(b->key_pressed & RIGHT)
	{
	  b->state=MOTION_STATE_INVERT_L2R;
	  b->clock=0;
	}
       else if(b->key_pressed & LEFT)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	  if(b->key_pressed & UP)
	    {
	      b->state=MOTION_STATE_RUN_JUMP_L;
	      b->clock=0;
	    }
	}
       else if(b->key_pressed & DOWN)
	{
	  b->state=MOTION_STATE_CROUCH_L;
	  b->clock=0;
	}
      else
	{
	  if(b->clock>0)
	    {
	      b->state=MOTION_STATE_BRAKE_L;
	      b->clock=0;
	    }
	  else
	    {
	      b->state=MOTION_STATE_STAND_L;
	    }
	}

      b->clock++;
      b->last_k_t=t;//boh
      break;
    case MOTION_STATE_RUN_R:
      if(!IS_ON_A_FLOOR(b->on_constraint))
	{
	  b->state=MOTION_STATE_FALL_R;
	  b->clock=0;
	  break;
	}
      if(b->key_pressed & LEFT)
	{
	  b->state=MOTION_STATE_INVERT_R2L;
	  b->clock=0;
	}
      else if(b->key_pressed & RIGHT)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	  if(b->key_pressed & UP)
	    {
	      b->state=MOTION_STATE_RUN_JUMP_R;
	      b->clock=0;
	    }
	}
      else if(b->key_pressed & DOWN)
	{
	  b->state=MOTION_STATE_CROUCH_R;
	  b->clock=0;
	}
      else
	{
	  if(b->clock>0)
	    {
	      b->state=MOTION_STATE_BRAKE_R;
	      b->clock=0;
	    }
	  else
	    {
	      b->state=MOTION_STATE_STAND_R;
	    }
	}

      b->clock++;
      b->last_k_t=t;//boh
      break;
    case MOTION_STATE_BRAKE_L:
    case MOTION_STATE_BRAKE_R:
      if(b->clock==7)
	{
	  if(b->state==MOTION_STATE_BRAKE_L)
	    {
	      b->state=MOTION_STATE_STAND_L;

	      /*if(b->key_pressed & RIGHT)
		{
		  b->state=MOTION_STATE_INVERT_L2R;
		}
		b->clock=0;*/
	    }
	  else
	    {
	      b->state=MOTION_STATE_STAND_R;

	      /*if(b->key_pressed & LEFT)
		{
		  b->state=MOTION_STATE_INVERT_R2L;
		}
		b->clock=0;*/
	    }
	}
      b->clock++;      
      break;

    case MOTION_STATE_INVERT_L2R:
      if(b->clock==8)
	{
	  b->state=MOTION_STATE_RUN_R;
	  b->clock=3;
	}
      b->clock++;
      break;
    case MOTION_STATE_INVERT_R2L:
      if(b->clock==8)
	{
	  b->state=MOTION_STATE_RUN_L;
	  b->clock=3;
	}
      b->clock++;
      break;
    case MOTION_STATE_CHANGE_DIR_L2R:
      if(b->clock==7)
	{
	  if(b->key_pressed & RIGHT)
	    {
	      b->state=MOTION_STATE_RUN_R;
	      b->clock=0;
	      b->acc[0]+=EXT_ACCEL_X;
	    }
	  else
	    {
	      b->state=MOTION_STATE_STAND_R;
	    }
	}
      b->clock++;
      break;
    case MOTION_STATE_CHANGE_DIR_R2L:
      if(b->clock==7)
	{
	  if(b->key_pressed & LEFT)
	    {
	      b->state=MOTION_STATE_RUN_L;
	      b->clock=0;
	      b->acc[0]-=EXT_ACCEL_X;
	    }
	  else
	    {
	      b->state=MOTION_STATE_STAND_L;
	    }
	}
      b->clock++;
      break;
    case MOTION_STATE_CROUCH_L:
    case MOTION_STATE_CROUCH_R:
      if(b->key_pressed & DOWN)
	{
	  if(b->clock!=4)
	    {
	      b->clock++;
	    }
	}
      else
	{
	  b->clock++;
	  if(b->clock==12)
	    {
	      b->state=(b->state==MOTION_STATE_CROUCH_L)?MOTION_STATE_STAND_L:MOTION_STATE_STAND_R;
	    }
	  
	}
      break;

    case MOTION_STATE_RUN_JUMP_L:
      if(b->clock<8)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      if(b->clock==5)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(b->clock==10)
	{
	  b->state=MOTION_STATE_RUN_L;
	  b->clock=0;
	}
      b->clock++;
      break;

    case MOTION_STATE_RUN_JUMP_R:
      if(b->clock<8)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      if(b->clock==5)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(b->clock==10)
	{
	  b->state=MOTION_STATE_RUN_R;
	  b->clock=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_JUMP_FWD_L:
      if(b->clock>5 && b->clock<8)
	{
	  b->acc[0]-=EXT_ACCEL_X;
	}
      if(b->clock==7)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(b->clock==16)
	{
	  if(IS_ON_A_FLOOR(b->on_constraint))
	    {
	      if(b->key_pressed & LEFT)
		b->state=MOTION_STATE_RUN_L;
	      else
		b->state=MOTION_STATE_STAND_L;
	    }
	  else
	    b->state=MOTION_STATE_FALL_L;

	  b->clock=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_JUMP_FWD_R:
      if(b->clock>5 && b->clock<8)
	{
	  b->acc[0]+=EXT_ACCEL_X;
	}
      if(b->clock==7)
	{
	  b->acc[1]+=EXT_ACCEL_Y;
	}
      else if(b->clock==16)
	{
	  if(IS_ON_A_FLOOR(b->on_constraint))
	    {
	      if(b->key_pressed & RIGHT)
		b->state=MOTION_STATE_RUN_R;
	      else
		b->state=MOTION_STATE_STAND_R;
	    }
	  else
	    b->state=MOTION_STATE_FALL_R;

	  b->clock=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_STEP_L:
      if(b->clock==11)
	{
	  b->state=MOTION_STATE_STAND_L;
	}
      else if(b->clock==4)
	{
	  
	  if(level_close_to_down_edge_r(b->pos))
	    {
	      b->state=MOTION_STATE_STEP_DANG_L;
	      b->clock=0;
	      break;
	    }
	  else
	    {
	      b->acc[0]-=EXT_ACCEL_X;
	    }
	}

      b->clock++;
      break;
    case MOTION_STATE_STEP_R:
      if(b->clock==11)
	{
	  b->state=MOTION_STATE_STAND_R;
	}
      else if(b->clock==4)
	{
	  if(level_close_to_down_edge_l(b->pos))
	    {
	      b->state=MOTION_STATE_STEP_DANG_R;
	      b->clock=0;
	      break;
	    }
	  else
	    {
	      b->acc[0]+=EXT_ACCEL_X;
	    }
	}

      b->clock++;
      break;
    case MOTION_STATE_CLIMB_UP_L:   
      if(b->clock==16)
	{
	  b->state=MOTION_STATE_STAND_L;
	  b->pos[0]-=1000;
	  b->pos[1]+=12000;
	  
	  b->suspend_dynamics=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_CLIMB_UP_R:
      if(b->clock==16)
	{
	  b->state=MOTION_STATE_STAND_R;
	  b->pos[0]+=1000;
	  b->pos[1]+=12000;
	  
	  b->suspend_dynamics=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_CLIMB_DOWN_L:   
      if(b->clock==16)
	{
	  b->state=MOTION_STATE_STAND_L;
	  b->suspend_dynamics=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_CLIMB_DOWN_R:   
      if(b->clock==16)
	{
	  b->state=MOTION_STATE_STAND_R;
	  b->suspend_dynamics=0;
	}
      b->clock++;
      break;
    case MOTION_STATE_FALL_L:
      if(IS_ON_A_FLOOR(b->on_constraint))
	{
	  b->state=MOTION_STATE_CROUCH_L;
	  b->clock=0;
	  //b->state=MOTION_STATE_STAND_L;
	}
      else if(b->clock!=4)
	{
	  b->clock++;
	}
      break;
    case MOTION_STATE_FALL_R:
      if(IS_ON_A_FLOOR(b->on_constraint))
	{
	  b->state=MOTION_STATE_CROUCH_R;
	  b->clock=0;
	  //b->state=MOTION_STATE_STAND_L;
	}
      else if(b->clock!=4)
	{
	  b->clock++;
	}
      break;
    case MOTION_STATE_STEP_DANG_L:
      //if(b->clock==1)
	b->state=MOTION_STATE_STAND_L;
      //b->clock++;
      break;
    case MOTION_STATE_STEP_DANG_R:
      //if(b->clock==1)
	b->state=MOTION_STATE_STAND_R;
      //b->clock++;
      break;
    }

}

static void motion_apply_external_vel(body_t *b, uint32_t t)
{
  uint32_t i;
  
  b->vel[0]=0;

  for(i=0;i<4;i++)
    {
      switch(b->key_pressed & (1<<i))
	{
	case UP:
	  //b->vel[1]+=EXT_ACCEL_Y;
	  break;
	case DOWN:
	  //b->vel[1]-=EXT_ACCEL;
	  break;
	case RIGHT:
	  if(t-b->last_k_t>400)
	    {
	      b->vel[0]=EXT_VEL_X;
	      b->last_k_t=t;
	    }
	  break;
	case LEFT:
	  if(t-b->last_k_t>400)
	    {
	      b->vel[0]=-EXT_VEL_X; 
	      b->last_k_t=t;
	    }
	  break;
	case SHIFT:
	  
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

static void motion_integrate_vel_over_time(body_t *b, uint32_t dt, uint8_t coordinate_flag)
{
  /* compute the new speeds */
  if(coordinate_flag & 1)
    b->vel[0]+=b->acc[0]*dt;
  
  if(coordinate_flag & 2)
    b->vel[1]+=b->acc[1]*dt;
}

static void motion_integrate_pos_over_time(body_t *b, uint32_t dt, uint8_t coordinate_flag)
{
  /* compute the new positions */
  if(coordinate_flag & 1)
    b->pos[0]+=b->vel[0]*dt;
  if(coordinate_flag & 2)
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
  
  PRINTF("****\n");
  
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
	      PRINTF("floor constr\n");
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
	      PRINTF("wall constr\n");
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
	  positive_cross=b->pos[0]<=cb->pos[0] && pos_p[0]>=cb->pos[0] && ABS(b->pos[1]-cb->pos[1])<=cb->dim[1]/2;
	  negative_cross=b->pos[0]>=cb->pos[0] && pos_p[0]<=cb->pos[0] && ABS(b->pos[1]-cb->pos[1])<=cb->dim[1]/2;
  
	  //printf("char motion: pos:%d, neg:%d, mass=%d\n",positive_cross, negative_cross, cb->mass);

	  if(negative_cross || positive_cross)
	    {
	      /* we have an elastic collision: apply the formula to compute the new speeds from the conservation of kinetic energy and the momentum */
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
  b->ctrl=CTRL_ACC;
  b->mass=mass;
  b->on_constraint=0;
  b->key_pressed=NONE;
  b->state=MOTION_STATE_STAND_R;
  b->suspend_dynamics=0;
  b->last_k_t=0;
  b->last_t=SDL_GetTicks();
}

uint8_t motion_move_body(body_t* b, uint32_t t)
{
  uint32_t dt;

  dt=t-b->last_t;
  //if(dt<DT)
  //  return 0;
  
  PRINTF("-----%lx-main=%d----\ndt=%d ms\n",(long unsigned int)b,b==&(character_get_main()->body),dt);
  b->acc[0]=0;
  b->acc[1]=0;
  
  switch(b->ctrl)
    {
    case CTRL_ACC:
      PRINTF("CTRL_ACC active\n");
      motion_apply_external_acc(b,t);
      if(!b->suspend_dynamics)
	{
	  motion_apply_friction(b);
	  motion_apply_gravity(b);
	  motion_integrate_vel_over_time(b,dt,3);
	  motion_integrate_pos_over_time(b,dt,3);
	}
      break;
    case CTRL_VEL:
      PRINTF("CTRL_VEL active\n");
      motion_apply_friction(b);
      motion_apply_gravity(b);
      motion_apply_external_vel(b,t);
      motion_integrate_vel_over_time(b,dt,2);
      motion_integrate_pos_over_time(b,dt,3);
      break;
    case CTRL_POS:
      PRINTF("CTRL_POS not supported\n");
      break;
    }
  
  motion_apply_constraints(b,dt);
  motion_apply_characters(b,dt);
  
  
  PRINTF("position=%d,%d mm\n",b->pos[0],b->pos[1]);
  PRINTF("velocity=%.6f,%.6f m/s\n",b->vel[0],b->vel[1]);
  PRINTF("acc=%.3f,%.3f m/s^2\n",b->acc[0],b->acc[1]);
  
    

  b->last_t=t;

  return 1;
}


