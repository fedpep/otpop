#include <stdint.h>

typedef enum
{
  CHICKEN=0,
  SQUIRREL,
  DOG,
  RABBIT
} character_kind_t;


typedef struct
  {
    float pos[2];
    float pos_dot[2];
    float acc[2];
    uint16_t pose_id;
    character_kind_t kind;
    uint8_t life;
    uint8_t on_constraint;
  } character_t;


character_t* character_init(character_kind_t kind);
void character_set_pos(character_t *c, float x, float y);
void character_set_speed(character_t *c, float x_dot, float y_dot);
