#ifndef _MOTION_H_
#define _MOTION_H_

#include "keyboard.h"

#define G_ACC              (-0.3f)
#define EXT_ACCEL_X        (0.4f)
#define EXT_ACCEL_Y        (-G_ACC*6)
#define B_FRIC_AIR         (0.0025f)
#define B_FRIC_FLOOR       (0.006f)
#define K_HOOK             (0.0001f)
#define DT                 (50)

//#define ELASTIC_COLLISIONS

typedef struct
{
  int32_t pos[2];
  float vel[2];
  float acc[2];
  int32_t dim[2];
  int32_t mass;
  uint8_t on_constraint;
  uint32_t last_t;
} body_t;

void motion_set_pos(body_t *b, int32_t *pos);
void motion_set_vel(body_t *b, float *vel);
void motion_set_dim(body_t *b, int32_t *dim);
void motion_init_body(body_t *b, int32_t *dim, int32_t mass);
uint8_t motion_move_body(body_t* b, keyboard_key_t k);

#endif
