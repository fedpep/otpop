#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "level.h"
#include "graph.h"

#define ABS(x)  (((x)>=0)?(x):(-(x)))

typedef struct
{
  constraint_t *constraint_list;
  constraint_t *tail;
  uint32_t level_num;
} level_t;


static level_t current_level;


static void level_add_to_constraints_list(constraint_t* c)
{
  if(!current_level.tail)
    {
      current_level.constraint_list=c;
      current_level.tail=c;
      return;
    }

  current_level.tail->next=c;//constraint_list;
  current_level.tail=current_level.tail->next;


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

      if(!fscanf(fp, "(%d, %d, %d),",
		 &c->p_start[0],
		 &c->p_start[1],
		 (int*)&c->edge))
	  break;

      c->p_end[0]=c->p_start[0]+4000; 
      c->p_end[1]=c->p_start[1];
      //printf("start=(%d,%d),end=(%d,%d),edge=%d\n",c->p_start[0],c->p_start[1],c->p_end[0],c->p_end[1],c->edge);
      if(IS_A_FLOOR(c))
	{
	  c->figure_ptr=graph_init_lvl_figure(GROUND);
	}

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
  current_level.tail=NULL;
  level_populate_constraints();
}

constraint_t* level_get_constraint_list(void)
{
  return current_level.constraint_list;
}

uint8_t level_close_to_up_edge_l(int32_t *p)
{
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr) &&
	 IS_LEFT_EDGE(curr) && 
	 p[0]<curr->p_end[0] && p[0]>curr->p_start[0] && 
	 curr->p_end[1]>p[1] && curr->p_end[1]-p[1]<=12000)
	{
	  p[0]=curr->p_start[0];
	  return 1;
	}
      curr=curr->next;
    }
  
  return 0;
}

uint8_t level_close_to_up_edge_r(int32_t *p)
{
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr) &&
	 IS_RIGHT_EDGE(curr) && 
	 p[0]<curr->p_end[0] && p[0]>curr->p_start[0] && 
	 curr->p_end[1]>p[1] && curr->p_end[1]-p[1]<=12000)
	{
	  p[0]=curr->p_end[0];
	  return 1;
	}
      curr=curr->next;
    }
  
  return 0;
}

uint8_t level_close_to_down_edge_l(int32_t *p)
{
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr) && 
	 IS_LEFT_EDGE(curr) && 
	 (p[1]-curr->p_end[1])<1300 && p[0]<curr->p_end[0] && p[0]>curr->p_start[0])
	{   
	  //printf("close down edge L\n");
	  p[0]=curr->p_start[0]+1000;//curr->p_end[0]+1000;
	  return 1; 
	}
      
      curr=curr->next;
    }
  return 0;
}

/*{
  constraint_t *prec=NULL;
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr))
	{
	  
	  if(prec && prec->p_end[0]-curr->p_start[0]>10 && prec->p_end[0]>p[0] && prec->p_end[0]-p[0]<2500)
	    {
	      
	      p[0]=prec->p_end[0]+1000;
	      return 1;
	    }
	  
	}
      prec=curr;
      curr=curr->next;
    }
  return 0;
  }*/

uint8_t level_close_to_down_edge_r(int32_t *p)
{
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr) &&
	 IS_RIGHT_EDGE(curr) &&
	 (p[1]-curr->p_end[1])<1300 && p[0]>curr->p_start[0] && p[0]<curr->p_end[0])
	{   
	  //printf("close down edge R\n");
	  p[0]=curr->p_end[0]-1000;
	  return 1; 
	}
      
      curr=curr->next;
    }
  return 0;
}
/*{
  constraint_t *prec=NULL;
  constraint_t *curr=level_get_constraint_list();
  while(curr)
    {
      if(IS_A_FLOOR(curr))
	{
	  if(prec && curr->p_start[0]-prec->p_end[0]>10 && p[0]>prec->p_start[0] && p[0]-prec->p_start[0]<2500)
	    {
	      p[0]=prec->p_start[0]-1000;
	      return 1;
	    }
	}
      prec=curr;
      curr=curr->next;
    }
  return 0;
  }*/
