#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "level.h"


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
