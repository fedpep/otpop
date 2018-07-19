#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <SDL.h>
#include "types.h"


//#define FULLSCREEN

#define BACKGROUND_COLOR 0x221122

void graph_init(void);
void graph_update(void);
figure_t* graph_init_chr_figure(character_kind_t kind);
figure_t* graph_init_lvl_figure(level_kind_t kind);

#endif

