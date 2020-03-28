#include <SDL.h>
#include <string.h>
#include "character.h"
#include "level.h"
#include "graph.h"
#include "time.h"

#include "clip_prince.h"
#include "clip_guard.h"

#define WIDTH (640)
#define HEIGHT (480)
#define SCALE (100)

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define PRINTF printf
#else
#define PRINTF
#endif



typedef struct images_struct
{
  SDL_Surface *surf;
  char* image_path;
  struct images_struct *next;
} image_t;

static image_t *images;
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
	
  images=NULL;
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

static SDL_Surface* graph_load_image(char *image_path)
{
  image_t *img;
  img=images;
  while(img)
    {
      if(strcmp(image_path,img->image_path)==0)
	{
	  printf("%s already exists!\n", image_path);
	  return img->surf;
	}

      img=img->next;
    }


  img=images;
  images=(image_t*)malloc(sizeof(image_t));
  images->surf=SDL_LoadBMP(image_path);
  images->image_path=(char*)malloc(strlen(image_path)+1);
  strcpy(images->image_path,image_path);
  images->next=img;

  return images->surf;
}

figure_t* graph_init_chr_figure(character_kind_t kind)
{
  figure_t* fig_ptr;
  uint32_t colorkey;
  fig_ptr=(figure_t*) malloc(sizeof(figure_t));
 

  switch(kind)
    {
    case KID:
      fig_ptr->fig_surf=graph_load_image("./clips/clip_prince.bmp");
      clip_prince_init(fig_ptr);
      break;
    case GUARD:
      fig_ptr->fig_surf=graph_load_image("./clips/clip_guard.bmp");
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

  return fig_ptr;
}


figure_t* graph_init_lvl_figure(level_kind_t kind)
{
  figure_t* fig_ptr;
  uint32_t colorkey;
  fig_ptr=(figure_t*) malloc(sizeof(figure_t));
 

  switch(kind)
    {
    case GROUND:
      fig_ptr->fig_surf=graph_load_image("./clips/floor.bmp");
      
      break;
    
    }
  
  if(!fig_ptr->fig_surf)
    {
      fprintf( stderr, "Could not load image: %s\n", SDL_GetError() );
      return NULL;
    }
 
  
  colorkey=SDL_MapRGB(fig_ptr->fig_surf->format, 255,255,255);
  SDL_SetColorKey(fig_ptr->fig_surf, SDL_SRCCOLORKEY, colorkey);
  

  return fig_ptr;
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
  return (point_is_in_quadrant(c->p_start[0],c->p_start[1]) || point_is_in_quadrant(c->p_end[0],c->p_end[1]));
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
      //PRINTF("v line s=%d,e=%d\n",p_start[1],p_end[1]);
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
      //PRINTF("h line s=%d,e=%d\n",p_start[0],p_end[0]);
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

static SDL_Rect* graph_set_current_clip(character_t* c)
{
  static SDL_Rect clip;
  figure_t *fig=c->figure_ptr;
  const uint16_t *clip_indexes;
  
  fig->get_clip_indexes(c->state, &clip_indexes);

  if(fig->clip_start_index!=clip_indexes[0])
    {
      fig->clip_start_index=clip_indexes[0];      
    }
  
  fig->clip_current_index=fig->clip_start_index;
  
  if(clip_indexes[2])
    fig->clip_current_index-=c->clock%clip_indexes[1];
  else
    fig->clip_current_index+=c->clock%clip_indexes[1];
  
  clip.x=fig->clips[fig->clip_current_index].x;

  if(DIRECTION_IS_RIGHT(&c->body))
    {
      clip.x+=fig->l2r_x_offset;
    }

  clip.y=fig->clips[fig->clip_current_index].y;
  clip.w=fig->clips[fig->clip_current_index].w;
  clip.h=fig->clips[fig->clip_current_index].h;
  
  //printf ("current idx %d, x=%d, y=%d\n",fig->clip_current_index,clip.x,clip.y);
  return &clip;
}


static void graph_constraints_update(void)
{
  constraint_t* constraint;
  figure_t* fig_ptr;
  int16_t x,x_start,y_start,x_end,y_end;


  PRINTF("------\n");
  constraint=level_get_constraint_list();
  while(constraint)
    {
      fig_ptr=constraint->figure_ptr;

      
      //if(constraint_is_in_quadrant(constraint))
      if(IS_A_FLOOR(constraint))
	{
	  //PRINTF("  -->draw\n");

	  /* for now represent the constraints using white lines */
	  graph_draw_line(constraint->p_start,constraint->p_end,0xffffff);

	  /* TBD: this part plots the floor textures. Need to be rewritten at all considering the limits 
	     and of the constraint currently visible */
	  if(point_is_in_quadrant(constraint->p_start[0], constraint->p_start[1]) || point_is_in_quadrant(constraint->p_end[0], constraint->p_end[1]))
	    {
	      graph_calculate_screen_coordinates(constraint->p_start[0], constraint->p_start[1], &x_start, &y_start);
	      //PRINTF("in_quadrant: start=(%d,%d), end=(%d,%d), edge=%d, sstart=(%d,%d)\n",constraint->p_start[0],constraint->p_start[1],constraint->p_end[0],constraint->p_end[1],constraint->edge,x_start,y_start);
	    
	
	    
	      fig_ptr->fig_rect.w=fig_ptr->fig_surf->w;
	      fig_ptr->fig_rect.h=fig_ptr->fig_surf->h;
	    
	      fig_ptr->fig_rect.x=x_start-fig_ptr->fig_rect.w/4;
	      fig_ptr->fig_rect.y=y_start-fig_ptr->fig_rect.h/2;
	    
	      PRINTF("fig (on screen): (%d,%d;%d,%d)\n",fig_ptr->fig_rect.x,fig_ptr->fig_rect.y,fig_ptr->fig_rect.w,fig_ptr->fig_rect.h);
	      SDL_BlitSurface(fig_ptr->fig_surf, NULL, screen, &fig_ptr->fig_rect); 
	    }
	  else if(point_is_in_quadrant(constraint->p_start[0]-fig_ptr->fig_surf->w/4, constraint->p_start[1]))
	    {
	      graph_calculate_screen_coordinates(constraint->p_start[0], constraint->p_start[1], &x_start, &y_start);
	      fig_ptr->fig_rect.w=fig_ptr->fig_surf->w;///2;
	      fig_ptr->fig_rect.h=fig_ptr->fig_surf->h;
	    
	      fig_ptr->fig_rect.x=WIDTH-fig_ptr->fig_rect.w/4;
	      fig_ptr->fig_rect.y=y_start-fig_ptr->fig_rect.h/2;
	    
	      
	      PRINTF("fig (right edge): (%d,%d;%d,%d)\n",fig_ptr->fig_rect.x,fig_ptr->fig_rect.y,fig_ptr->fig_rect.w,fig_ptr->fig_rect.h);
	      SDL_BlitSurface(fig_ptr->fig_surf, NULL, screen, &fig_ptr->fig_rect);
	    }
	  else if(point_is_in_quadrant(constraint->p_end[0]+fig_ptr->fig_surf->w/4, constraint->p_end[1]))
	    {
	      graph_calculate_screen_coordinates(constraint->p_start[0], constraint->p_start[1], &x_start, &y_start);
	      fig_ptr->fig_rect.w=fig_ptr->fig_surf->w;
	      fig_ptr->fig_rect.h=fig_ptr->fig_surf->h;
	    
	      fig_ptr->fig_rect.x=-3*fig_ptr->fig_rect.w/4;
	      fig_ptr->fig_rect.y=y_start-fig_ptr->fig_rect.h/2;
	    
	      PRINTF("fig (left edge): (%d,%d;%d,%d)\n",fig_ptr->fig_rect.x,fig_ptr->fig_rect.y,fig_ptr->fig_rect.w,fig_ptr->fig_rect.h);
	      SDL_BlitSurface(fig_ptr->fig_surf, NULL, screen, &fig_ptr->fig_rect);
	    }
	}
      constraint=constraint->next;
    }

}


void graph_characters_update(void)
{
  character_t* character;
  figure_t* fig_ptr;
  SDL_Rect *current_clip;

  character=character_get_list();
  while(character)
    {
      if(character_is_in_quadrant(character))
	{
	  fig_ptr = character->figure_ptr;
	  graph_calculate_screen_coordinates(character->body.pos[0],character->body.pos[1],&fig_ptr->fig_rect.x,&fig_ptr->fig_rect.y);

	  current_clip=graph_set_current_clip(character);

	  fig_ptr->fig_rect.w=current_clip->w;
	  fig_ptr->fig_rect.h=current_clip->h;
	  fig_ptr->fig_rect.x-=fig_ptr->fig_rect.w/2;
	  fig_ptr->fig_rect.y-=fig_ptr->fig_rect.h;

	  SDL_BlitSurface(fig_ptr->fig_surf, current_clip, screen, &fig_ptr->fig_rect);
	
	}
      character=character->next;
    }
  
}

void graph_update(void)
{  

  SDL_FillRect(screen, NULL, BACKGROUND_COLOR);
  
  graph_update_quadrant(character_get_main()->body.pos);

  
  graph_constraints_update();
  graph_characters_update();
  
  SDL_Flip(screen);  
}
