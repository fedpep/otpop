#include <SDL.h>
#include <SDL_keysym.h>
#include <SDL_events.h>
#include "keyboard.h"


typedef struct
{
  SDLKey sdl_event;
  keyboard_key_t event;
} keymaps_t; 

#define KEYM_SIZE (sizeof(keym)/sizeof(keymaps_t))
 
static keyboard_key_t current_key=NONE;

static keymaps_t keym[]={{SDLK_DOWN, DOWN},
			 {SDLK_UP, UP},
			 {SDLK_LEFT, LEFT},
			 {SDLK_RIGHT, RIGHT},
			 {SDLK_LSHIFT,SHIFT},
			 {SDLK_RSHIFT,SHIFT}
};

keyboard_key_t keyboard_check(void)
{
  SDL_Event event;
  int i;

  if(SDL_PollEvent(&event))
    {
      //printf("%d\n",KEYM_SIZE);

      if(event.type==SDL_KEYDOWN || event.type==SDL_KEYUP)
	{
	  if(event.type==SDL_KEYDOWN && (event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.sym==SDLK_c || event.key.keysym.sym==SDLK_q))
	    {
	      printf("bye!\n");
	      SDL_Quit();
	      exit(0);
	    }
	  for(i=0;i<KEYM_SIZE;i++)
	    {
	      if(keym[i].sdl_event==event.key.keysym.sym)
		{
		  if(event.type==SDL_KEYDOWN)
		    current_key |= keym[i].event;
		  else if(event.type==SDL_KEYUP)
		    current_key &= ~keym[i].event;
		}
	    }
	}
    }
  return current_key;
  
}


/*
keyboard_key_t keyboard_check(void)
{
  SDL_Event event;
  int i;

  if(SDL_PollEvent(&event))
    {
      //printf("%d\n",KEYM_SIZE);

      if(event.type==SDL_KEYDOWN)
	{
	  if((event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.sym==SDLK_c))
	    {
	      printf("bye!\n");
	      SDL_Quit();
	      exit(0);
	    }
	  for(i=0;i<KEYM_SIZE;i++)
	    {
	      if(keym[i].sdl_event==event.key.keysym.sym)
		{
		  return keym[i].event;
		}
	    }
	}
    }
  return NONE;
  
}
*/
