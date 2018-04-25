#include "graph.h"
#include "keyboard.h"
#include "level.h"

#include <stdio.h>


void main(void)
{
  keyboard_key_t k;
  character_t* c=character_init(SQUIRREL);
  
  level_init(1);

  graph_init();
  
  while(1)
    {
      k=keyboard_check();
      motion_move_character(c,k);
      graph_update(c);
    }
  

}
