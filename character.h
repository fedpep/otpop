#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <stdint.h>
#include "types.h"
#include "motion.h"
#include "graph.h"


character_t* character_init(character_kind_t kind);
character_t* character_init_main(character_kind_t kind);
character_t *character_get_list(void);
character_t* character_get_main(void);
void character_state_tick(character_t *c);
void character_set_initial_state(character_t *c, uint32_t *pos, float *vel, direction_t dir);

#endif
