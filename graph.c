#include <SDL.h>
#include "character.h"
#include "level.h"
#include "graph.h"

#define WIDTH (640)
#define HEIGHT (480)
#define SCALE (100)

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif


static SDL_Surface *screen=NULL;
static uint32_t quadrant[2]={0,0};

typedef struct
{
  SDL_Surface *fig_surf;
  SDL_Rect fig_rect;
} figure_t;



void graph_init(void)
{
  uint32_t flags=SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_ANYFORMAT;

#ifdef FULLSCREEN
  flags|=SDL_FULLSCREEN;
#endif

  /* Initialise SDL */
  if( SDL_Init( SDL_INIT_VIDEO ) < 0){
    fprintf( stderr, "Could not initialise SDL: %s\n", SDL_GetError() );
    exit( -1 );
  }
  

  screen=SDL_SetVideoMode( WIDTH, HEIGHT, 32 , flags);
			   
  SDL_ShowCursor(SDL_DISABLE);

  /* Set a video mode */
  if( !screen )
    {
      fprintf( stderr, "Could not set video mode: %s\n", SDL_GetError() );
      SDL_Quit();
      exit( -1 );
    }

  /* Enable Unicode translation */
  SDL_EnableUNICODE( 1 );

  
  atexit(SDL_Quit);

}

void* graph_init_figure(character_kind_t kind)
{
  figure_t* fig_ptr;

  fig_ptr=(figure_t*) malloc(sizeof(figure_t));

  switch(kind)
    {
    case KID:
      fig_ptr->fig_surf=SDL_LoadBMP("./blue_rect.bmp");
      break;
    case GUARD:
      fig_ptr->fig_surf=SDL_LoadBMP("./orange_rect.bmp");
      break;
    case VIZIR:
      fig_ptr->fig_surf=SDL_LoadBMP("./red_rect.bmp");
      break;
    }
  if(!fig_ptr->fig_surf)
    {
      fprintf( stderr, "Could not load image: %s\n", SDL_GetError() );
      return NULL;
    }
 
  
  return (void*)fig_ptr;
}


static uint8_t point_is_in_quadrant(int32_t x, int32_t y)
{
  int32_t x0,y0;
  x0=quadrant[0]*WIDTH*SCALE;
  y0=quadrant[1]*HEIGHT*SCALE;
  
  //PRINTF("xm=%d,ym=%d\n",x0,y0);
  return ((x>x0 && x<x0+WIDTH*SCALE) && \
	   (y>y0 && y<y0+HEIGHT*SCALE));
}


static uint8_t constraint_is_in_quadrant(constraint_t* c)
{ 
  /* TBF: this is not covering the case the line extreme points are in 2 different quandrant than this one */
  return 1;//(point_is_in_quadrant(c->p_start[0],c->p_start[1]) || point_is_in_quadrant(c->p_end[0],c->p_end[1]));
}

static uint8_t character_is_in_quadrant(character_t* c)
{ 
  return point_is_in_quadrant(c->body.pos[0],c->body.pos[1]);
}

static void graph_put_pixel(uint32_t x, uint32_t y, uint32_t pixel) {
  uint32_t byteperpixel = screen->format->BytesPerPixel;
  uint8_t *p;
  if(x>0 && x<WIDTH && y>0 && y<HEIGHT)
    {
      p = (uint8_t*)screen->pixels + y * screen->pitch + x * byteperpixel;
      // Adress to pixel
      *(uint32_t *)p = pixel;
    }
}


static void graph_calculate_screen_coordinates(int32_t x, int32_t y, int16_t *sx, int16_t *sy)
{
  *sx=(int16_t)((int32_t)x/SCALE)%WIDTH;
  *sy=(int16_t)((HEIGHT-(int32_t)y/SCALE)%HEIGHT);
}

static void graph_draw_line(int32_t* p_start, int32_t* p_end, uint32_t pixel) {

  int i;
  int16_t x,y;

  if(p_start[0]==p_end[0])
    {
      PRINTF("v line s=%d,e=%d\n",p_start[1],p_end[1]);
      /*vertical*/
      for(i=p_start[1];i<=p_end[1];i++)
	{
	  if(point_is_in_quadrant(p_start[0],i))
	    {
	      graph_calculate_screen_coordinates(p_start[0],i,&x,&y);
	      graph_put_pixel(x, y, pixel);
	    }
	}
    }
  else if(p_start[1]==p_end[1])
    {
      PRINTF("h line s=%d,e=%d\n",p_start[0],p_end[0]);
      /*horizontal*/
      for(i=p_start[0];i<=p_end[0];i++)
	{
	  if(point_is_in_quadrant(i,p_start[1]))
	    {
	      graph_calculate_screen_coordinates(i,p_start[1],&x,&y);
	      graph_put_pixel(x, y, pixel);
	    }
	}
    }
    

}

void graph_update_quadrant(int32_t *pos)
{
  quadrant[0]=pos[0]/SCALE/WIDTH;
  quadrant[1]=pos[1]/SCALE/HEIGHT;

  if(pos[0]<0) quadrant[0]--;
  if(pos[1]<0) quadrant[1]--;
  
  PRINTF("quadrant (%d,%d)\n",quadrant[0],quadrant[1]);
}



void graph_update(void)
{
  constraint_t* constraint;
  character_t* character=character_get_main();
  figure_t* fig_ptr = (figure_t*)character->figure_ptr;

  SDL_FillRect(screen, NULL, 0x221122);
  
  graph_update_quadrant(character->body.pos);

  graph_calculate_screen_coordinates(character->body.pos[0],character->body.pos[1],&fig_ptr->fig_rect.x,&fig_ptr->fig_rect.y);
  fig_ptr->fig_rect.x-=fig_ptr->fig_rect.w/2;
  fig_ptr->fig_rect.y-=fig_ptr->fig_rect.h;
  SDL_BlitSurface(fig_ptr->fig_surf , NULL , screen , &fig_ptr->fig_rect);

  character=character_get_list();
  while(character)
    {
      if(character_is_in_quadrant(character))
	{
	  fig_ptr = (figure_t*)character->figure_ptr;
	  graph_calculate_screen_coordinates(character->body.pos[0],character->body.pos[1],&fig_ptr->fig_rect.x,&fig_ptr->fig_rect.y);
	  fig_ptr->fig_rect.x-=fig_ptr->fig_rect.w/2;
	  fig_ptr->fig_rect.y-=fig_ptr->fig_rect.h;
	  SDL_BlitSurface(fig_ptr->fig_surf , NULL , screen , &fig_ptr->fig_rect);
	  if(character->state==FIGHT)
	    {
	      SDL_FillRect( screen, &fig_ptr->fig_rect, 0xFF0000);
	    }
	}
      character=character->next;
    }
  
  PRINTF("------\n");
  constraint=level_get_constraint_list();
  while(constraint)
    {
      PRINTF("%d,%d, %d,%d\n",constraint->p_start[0],constraint->p_start[1],constraint->p_end[0],constraint->p_end[1]);
      if(constraint_is_in_quadrant(constraint))
	{
	  PRINTF("  -->draw\n");

	  graph_draw_line(constraint->p_start,constraint->p_end,0xffffff);
	}
      constraint=constraint->next;
    }

  
  SDL_Flip(screen);
  
}
