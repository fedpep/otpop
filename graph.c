#include <SDL.h>
#include "character.h"
#include "level.h"
#include "graph.h"
#include "time.h"

#include "clip_prince.h"
#include "clip_guard.h"

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
  uint32_t colorkey;
  fig_ptr=(figure_t*) malloc(sizeof(figure_t));
  fig_ptr->clip_current_index=0;

  switch(kind)
    {
    case KID:
      fig_ptr->fig_surf=SDL_LoadBMP("./clips/clip_prince.bmp");//"./blue_rect.bmp");
      clip_prince_init(fig_ptr);
      break;
    case GUARD:
      fig_ptr->fig_surf=SDL_LoadBMP("./clips/clip_guard.bmp");//"./blue_rect.bmp");
      clip_guard_init(fig_ptr);
      break;
    }
  
  
  fig_ptr->clip_current_index=0;
  fig_ptr->clip_start_index=0;
  
  if(!fig_ptr->fig_surf)
    {
      fprintf( stderr, "Could not load image: %s\n", SDL_GetError() );
      return NULL;
    }
 
  
  colorkey=SDL_MapRGB(fig_ptr->fig_surf->format, 255,255,255);
  SDL_SetColorKey(fig_ptr->fig_surf, SDL_SRCCOLORKEY, colorkey);

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

static void graph_set_clip(character_t* c)
{
  figure_t *fig=c->figure_ptr;
  int *clip_indexes;
  uint16_t clock;
  

  fig->get_clip_indexes(c->state, DIRECTION_IS_RIGHT(&c->body), &clip_indexes);

  if(fig->clip_start_index!=clip_indexes[0])
    {
      fig->clip_start_index=clip_indexes[0];      
    }
  
  fig->clip_current_index=fig->clip_start_index;
  
  if(clip_indexes[2])
    fig->clip_current_index-=c->clock;
  else
    fig->clip_current_index+=c->clock;
  
}


void graph_update(void)
{
  constraint_t* constraint;
  character_t* character;
  figure_t* fig_ptr;
  uint32_t color;
  SDL_Rect clip;

  SDL_FillRect(screen, NULL, 0x221122);
  
  graph_update_quadrant(character_get_main()->body.pos);

  character=character_get_list();
  while(character)
    {
      if(character_is_in_quadrant(character))
	{
	  fig_ptr = character->figure_ptr;
	  graph_calculate_screen_coordinates(character->body.pos[0],character->body.pos[1],&fig_ptr->fig_rect.x,&fig_ptr->fig_rect.y);
	  /*fig_ptr->fig_rect.x-=fig_ptr->fig_rect.w/2;
	  fig_ptr->fig_rect.y-=fig_ptr->fig_rect.h;*/

	  graph_set_clip(character);

	  fig_ptr->fig_rect.x-=(fig_ptr->clips[fig_ptr->clip_current_index].w/2);//*100/SCALE;
	  fig_ptr->fig_rect.y-=(fig_ptr->clips[fig_ptr->clip_current_index].h);//*100/SCALE;


	  SDL_BlitSurface(fig_ptr->fig_surf , &fig_ptr->clips[fig_ptr->clip_current_index] , screen , &fig_ptr->fig_rect);
	
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
