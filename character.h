#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <stdint.h>
#include "types.h"
#include "motion.h"
#include "graph.h"

#define IS_FIGHTING(C)  (C->state==IN_GUARD || C->state==ATTACK || C->state==DEFENSE)


character_t* character_init(character_kind_t kind);
character_t* character_init_main(character_kind_t kind);
character_t *character_get_list(void);
character_t* character_get_main(void);
//void character_state_check(character_t *c, uint32_t t);

#endif
