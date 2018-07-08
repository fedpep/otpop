#include <SDL.h>

uint32_t time_get_now(void)
{
  return SDL_GetTicks();
}
