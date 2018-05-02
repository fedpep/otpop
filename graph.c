#include <SDL.h>
#include "character.h"
#include "level.h"

#define WIDTH 640
#define HEIGHT 480
#define SCALE 100

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif


static SDL_Surface *screen=NULL,*logo;
static uint32_t quadrant[2]={0,0};
static SDL_Rect logoRect;


void graph_init(void)
{
  /* Initialise SDL */
  if( SDL_Init( SDL_INIT_VIDEO ) < 0){
    fprintf( stderr, "Could not initialise SDL: %s\n", SDL_GetError() );
    exit( -1 );
  }
  
  screen=SDL_SetVideoMode( WIDTH, HEIGHT, 32 , SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_ANYFORMAT);

  /* Set a video mode */
  if( !screen )
    {
      fprintf( stderr, "Could not set video mode: %s\n", SDL_GetError() );
      SDL_Quit();
      exit( -1 );
    }

  /* Enable Unicode translation */
  SDL_EnableUNICODE( 1 );

  //logo = SDL_LoadBMP("./ball.bmp");
  logo=SDL_LoadBMP("./logo.bmp");
  if(!logo)
    {
      fprintf( stderr, "Could not load image: %s\n", SDL_GetError() );
      SDL_Quit();
      exit( -1 );
    }
 
  
  atexit(SDL_Quit);

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
  return (point_is_in_quadrant(c->p_start[0],c->p_start[1]) || point_is_in_quadrant(c->p_end[0],c->p_end[1]));
}


static void graph_put_pixel(int x, int y, uint32_t pixel) {
    int byteperpixel = screen->format->BytesPerPixel;
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
      PRINTF("v\n");
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
      PRINTF("h\n");
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

  constraint=level_get_constraint_list();

  SDL_FillRect(screen, NULL, 0x221122);
  
  graph_update_quadrant(character->body.pos);
  graph_calculate_screen_coordinates(character->body.pos[0],character->body.pos[1],&logoRect.x,&logoRect.y);
  logoRect.x-=logoRect.w/2;
  logoRect.y-=logoRect.h;
  
  
  PRINTF("------\n");
  while(constraint)
    {
      if(constraint_is_in_quadrant(constraint))
	{
	  PRINTF("draw\n");
	  graph_draw_line(constraint->p_start,constraint->p_end,0xffffff);
	}
      constraint=constraint->next;
    }

  SDL_BlitSurface( logo , NULL , screen , &logoRect );
  SDL_Flip(screen);
  
}
