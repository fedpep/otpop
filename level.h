#include "types.h"


#define IS_ON_A_WALL(OC)   (OC & ON_A_WALL)
#define IS_ON_A_FLOOR(OC)  (OC & ON_A_FLOOR)


#define IS_A_FLOOR(C_POINT) ((C_POINT)->p_start[1]==(C_POINT)->p_end[1])
#define IS_A_WALL(C_POINT)  ((C_POINT)->p_start[0]==(C_POINT)->p_end[0])

void level_init(uint32_t num);
constraint_t* level_get_constraint_list(void);
uint8_t level_close_to_up_edge_l(int32_t *p);
uint8_t level_close_to_up_edge_r(int32_t *p);
uint8_t level_close_to_down_edge_l(int32_t *p);
uint8_t level_close_to_down_edge_r(int32_t *p);


