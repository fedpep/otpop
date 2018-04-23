#include <SDL.h>
#include "character.h"
#include <math.h>
#include "level.h"

#define WIDTH 640
#define HEIGHT 480
#define SCALE 100

extern constraint_t constr_temp[4];

static SDL_Surface *screen=NULL,*logo;
SDL_Rect logoRect;


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

  logo = SDL_LoadBMP("./logo.bmp");
  if(!logo)
    {
      fprintf( stderr, "Could not load image: %s\n", SDL_GetError() );
      SDL_Quit();
      exit( -1 );
    }
 
  
  atexit(SDL_Quit);

}


void putpixel(SDL_Surface *theScreen, int x, int y, uint32_t pixel) {
    int byteperpixel = theScreen->format->BytesPerPixel;

    Uint8 *p = (Uint8*)theScreen->pixels + y * theScreen->pitch + x * byteperpixel;

    // Adress to pixel
    *(Uint32 *)p = pixel;
}


void drawLine(SDL_Surface *Screen, int x0, int y0, int x1, int y1, uint32_t pixel) {

  int i;
    double x = x1 - x0; 
	double y = y1 - y0; 
	double length = sqrt( x*x + y*y ); 
	double addx = x / length; 
	double addy = y / length; 
	x = x0; 
	y = y0; 
	
	for ( i = 0; i < length; i += 1) { 
		putpixel(Screen, x, y, pixel ); 
		x += addx; 
		y += addy; 
		
	} 
}

void graph_update(character_t *c)
{
  int i;
  constraint_t* constraint;

  constraint=level_get_constraint_list();

  SDL_FillRect(screen, NULL, 0x221122);
  
  logoRect.x=((int)c->pos[0]/SCALE)%WIDTH-logoRect.w/2;
  logoRect.y=((HEIGHT-(int)c->pos[1]/SCALE)%HEIGHT)-logoRect.h;
  
  while(constraint)
    {
      drawLine(screen, constraint->p_start[0]/SCALE,HEIGHT-constraint->p_start[1]/SCALE,constraint->p_end[0]/SCALE,HEIGHT-constraint->p_end[1]/SCALE,0xffffff);
      constraint=constraint->next;
    }

  SDL_BlitSurface( logo , NULL , screen , &logoRect );
  SDL_Flip(screen);
  
}
