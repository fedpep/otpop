#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "level.h"

#define ABS(x)  (((x)>=0)?(x):(-(x)))

typedef struct
{
  constraint_t *constraint_list;
  uint32_t level_num;
} level_t;


static level_t current_level;


static void level_add_to_constraints_list(constraint_t* c)
{
  constraint_t* aux;
  aux=current_level.constraint_list;
  current_level.constraint_list=c;
  c->next=aux;
}

static void level_populate_constraints(void)
{
  FILE* fp;
  char file_name[20];
  constraint_t* c;
  int i=0;

  sprintf(file_name, "level_%d.otpp", current_level.level_num);
  fp=fopen(file_name,"r");
  
  while(!feof(fp))
    {
      c=(constraint_t*)malloc(sizeof(constraint_t));

      if(!fscanf(fp, "%d %d %d %d\n",
		 &c->p_start[0],
		 &c->p_start[1], 
		 &c->p_end[0], 
		 &c->p_end[1]))	
	  break;

      level_add_to_constraints_list(c);

      printf("constr: %d, %d, %d, %d\n",c->p_start[0],
	     c->p_start[1], 
	     c->p_end[0], 
	     c->p_end[1]);      
    }

  fclose(fp);

}

void level_init(uint32_t num)
{
  current_level.level_num=num;
  current_level.constraint_list=NULL;
  
  level_populate_constraints();
}

constraint_t* level_get_constraint_list(void)
{
  return current_level.constraint_list;
}

uint8_t level_close_to_up_edge_l(int32_t *p)
{
  constraint_t *c=level_get_constraint_list();
  while(c)
    {
      if(IS_A_FLOOR(c))
	{

	  if(p[0]>c->p_end[0] && p[0]-c->p_end[0]<2500 && c->p_end[1]>p[1] && c->p_end[1]-p[1]<=12000)
	    {
	      //p[0]=c->p_end[0]-1000;
	      return 1;
	    }
	}
      c=c->next;
    }
  
  return 0;
}

uint8_t level_close_to_up_edge_r(int32_t *p)
{
  constraint_t *c=level_get_constraint_list();
  while(c)
    {
      if(IS_A_FLOOR(c))
	{
	  if(p[0]<c->p_start[0] && c->p_start[0]-p[0]<2500 && c->p_end[1]>p[1] && c->p_end[1]-p[1]<=12000)
	    {
	      //p[0]=c->p_start[0]+1000;
	      return 1;
	    }
	}
      c=c->next;
    }
  
  return 0;
}

uint8_t level_close_to_down_edge_l(int32_t *p)
{
  constraint_t *c=level_get_constraint_list();
  while(c)
    {
      if(IS_A_FLOOR(c))
	{
	  if(c->p_end[0]>p[0] && c->p_end[0]-p[0]<2500)
	    {
	      //p[0]=c->p_end[0]+1000;
	      return 1;
	    }
	}
      c=c->next;
    }
  return 0;
}

uint8_t level_close_to_down_edge_r(int32_t *p)
{
  constraint_t *c=level_get_constraint_list();
  while(c)
    {
      if(IS_A_FLOOR(c))
	{
	  if(p[0]>c->p_start[0] && p[0]-c->p_start[0]<2500)
	    {
	      //p[0]=c->p_start[0]-1000;
	      return 1;
	    }
	}
      c=c->next;
    }
  return 0;
}
