#include <SDL.h>

uint32_t time_get_now(void)
{
  return SDL_GetTicks();
}

void time_delay(uint32_t ms)
{
  SDL_Delay(ms);
}
