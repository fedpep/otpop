#define G_ACC         (-0.3f)
#define EXT_ACCEL_X     (0.4f)
#define EXT_ACCEL_Y     (-G_ACC*6)
#define B_FRIC_AIR        (0.003f)
#define B_FRIC_FLOOR      (0.008f)
#define K_HOOK        (0.0001f)
#define DT            (50)

//#define ELASTIC_COLLISIONS


void motion_move_character(character_t* c, keyboard_key_t k);
