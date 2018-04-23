#include <stdint.h>

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define NONE  0 
#define UP    0x01  // when the shift amount is even it is a +
#define DOWN  0x02  // when the shift amount is even it is a -
#define RIGHT 0x04
#define LEFT  0x08

#define SHIFT  0x10

typedef uint32_t keyboard_key_t;

keyboard_key_t keyboard_init(void);

#endif /*_KEYBOARD_H_*/
