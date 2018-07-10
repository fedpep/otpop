#include <SDL.h>
#include "character.h"
#include "level.h"
#include "graph.h"
#include "time.h"


#include "clip_prince.h"

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
  fig_ptr->last_t=0;



  switch(kind)
    {
    default://case //KID:
      fig_ptr->fig_surf=SDL_LoadBMP("./clips/clip_prince.bmp");//"./blue_rect.bmp");
      fig_ptr->clips=(SDL_Rect*)malloc(sizeof(SDL_Rect));
      clip_prince_init(fig_ptr);
      fig_ptr->clip_current_index=0;
      fig_ptr->clip_start_index=0;
      break;
      /*    case GUARD:
      fig_ptr->fig_surf=SDL_LoadBMP("./low.bmp");//"./orange_rect.bmp");
      break;
    case VIZIR:
      fig_ptr->fig_surf=SDL_LoadBMP("./low.bmp");//"./red_rect.bmp");
      break;*/
    }
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

static void graph_set_clip(character_t* character)
{
  figure_t *fig=character->figure_ptr;
  body_t *b=&character->body;
  uint32_t t=time_get_now();

  fig->last_t=t;

  switch(b->state)
    {
    case MOTION_STATE_STAND_L:
      fig->clip_start_index=CLIP_PRINCE_ROW_0;
      fig->clip_current_index=fig->clip_start_index;
      break;
    case MOTION_STATE_STAND_R:
      fig->clip_start_index=CLIP_PRINCE_ROW_0_H;
      fig->clip_current_index=fig->clip_start_index;
      break;
    case MOTION_STATE_JUMP_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_2)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_2;
	  fig->clip_current_index=fig->clip_start_index;
	}      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_JUMP_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_2_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_2_H;
	  fig->clip_current_index=fig->clip_start_index;
	}      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_RUN_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_1+8)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_1+8;
	  fig->clip_current_index=fig->clip_start_index+b->clock;
	}
      fig->clip_current_index++;
      if(fig->clip_current_index==CLIP_PRINCE_ROW_1+20)
	fig->clip_current_index=fig->clip_start_index+5;

      break;
    case MOTION_STATE_RUN_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_1_H+8)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_1_H+8;
	  fig->clip_current_index=fig->clip_start_index+b->clock;
	}
      fig->clip_current_index++;
      if(fig->clip_current_index==CLIP_PRINCE_ROW_1_H+20)
	fig->clip_current_index=fig->clip_start_index+5;

      break;
    case MOTION_STATE_BRAKE_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_1)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_1;
	  fig->clip_current_index=fig->clip_start_index;
	}
      fig->clip_current_index++;
      break;
    case MOTION_STATE_BRAKE_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_1_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_1_H;
	  fig->clip_current_index=fig->clip_start_index;
	}
      fig->clip_current_index++;
      
      break;
    case MOTION_STATE_INVERT_R2L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_0_H+9)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_0_H+9;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_INVERT_L2R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_0+9)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_0+9;
	}
      fig->clip_current_index=fig->clip_start_index+b->clock;
      
      break;
    case MOTION_STATE_CHANGE_DIR_L2R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_0+1)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_0+1;
	}
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_CHANGE_DIR_R2L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_0_H+1)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_0_H+1;
	}
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;

    case MOTION_STATE_CROUCH_L:
      fig->clip_start_index=CLIP_PRINCE_ROW_4;
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_CROUCH_R:
      fig->clip_start_index=CLIP_PRINCE_ROW_4_H;
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
      
    case MOTION_STATE_RUN_JUMP_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_6)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_6;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;

    case MOTION_STATE_RUN_JUMP_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_6_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_6_H;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_JUMP_FWD_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_5)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_5;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_JUMP_FWD_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_5_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_5_H;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_STEP_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_7)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_7;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_STEP_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_7_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_7_H;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_CLIMB_UP_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_3)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_3;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_CLIMB_UP_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_3_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_3_H;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_CLIMB_DOWN_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_3+16)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_3+16;
	}
      
      fig->clip_current_index=fig->clip_start_index-b->clock;
      break;
    case MOTION_STATE_CLIMB_DOWN_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_3_H+16)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_3_H+16;
	}
      
      fig->clip_current_index=fig->clip_start_index-b->clock;
      break;
    case MOTION_STATE_FALL_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_8)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_8;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_FALL_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_8_H)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_8_H;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_STEP_DANG_L:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_4+15)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_4+15;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    case MOTION_STATE_STEP_DANG_R:
      if(fig->clip_start_index!=CLIP_PRINCE_ROW_4_H+15)
	{
	  fig->clip_start_index=CLIP_PRINCE_ROW_4_H+15;
	}
      
      fig->clip_current_index=fig->clip_start_index+b->clock;
      break;
    }

  
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

	  fig_ptr->fig_rect.x-=fig_ptr->clips[fig_ptr->clip_current_index].w/2;
	  fig_ptr->fig_rect.y-=fig_ptr->clips[fig_ptr->clip_current_index].h;

	  
	  //printf("chr %x, indx %d\n",(uint32_t)character,fig_ptr->clip_current_index);

	  SDL_BlitSurface(fig_ptr->fig_surf , &fig_ptr->clips[fig_ptr->clip_current_index] , screen , &fig_ptr->fig_rect);
	  /*
	  if(IS_FIGHTING(character))
	    {
	      color=0xff0000;
	      if(character==character_get_main())
		{
		  color=0xfff000;
		}
	      SDL_FillRect( screen, &fig_ptr->fig_rect, color);
	      }*/
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
