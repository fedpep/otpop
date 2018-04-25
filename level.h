typedef struct constraint_struct
{
  int32_t p_start[2];
  int32_t p_end[2];
  struct constraint_struct *next;
} constraint_t;

#define ON_A_FLOOR 0x01
#define ON_A_WALL  0x02

#define IS_ON_A_WALL(OC)   (OC & ON_A_WALL)
#define IS_ON_A_FLOOR(OC)  (OC & ON_A_FLOOR)


#define IS_A_FLOOR(C_POINT) ((C_POINT)->p_start[1]==(C_POINT)->p_end[1])
#define IS_A_WALL(C_POINT)  ((C_POINT)->p_start[0]==(C_POINT)->p_end[0])


void level_init(uint32_t num);
constraint_t* level_get_constraint_list(void);

