#ifndef _MOTION_H_
#define _MOTION_H_

#include "types.h"
#include "keyboard.h"

#define G_ACC              (-0.3f)
#define EXT_ACCEL_X        (0.3f)
#define EXT_ACCEL_Y        (-G_ACC*3)
#define EXT_VEL_X        (40.0f)
//#define EXT_VEL_Y        (-G_ACC*4)
#define B_FRIC_AIR         (0.0025f)
#define B_FRIC_FLOOR       (0.008f)//(0.006f)
#define K_HOOK             (0.0001f)
#define DT                 (50)

//#define ELASTIC_COLLISIONS


void motion_set_pos(body_t *b, int32_t *pos);
void motion_set_vel(body_t *b, float *vel);
void motion_set_dim(body_t *b, int32_t *dim);
void motion_init_body(body_t *b, int32_t *dim, int32_t mass);
uint8_t motion_move_body(body_t* b, uint32_t t);
uint8_t motion_body_close_l(body_t *b1, body_t *b2, uint32_t distance);
uint8_t motion_body_close_r(body_t *b1, body_t *b2, uint32_t distance);

#endif
