#ifndef DEFNS_H
#define DEFNS_H
#include <SDL2/SDL.h>

struct graphics_SDL{
    SDL_Window* window;
    SDL_Surface* surface;
};

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320
#define MULTIPLIER(real, emu) (real/emu)

#define MEMORY_SIZE 4096
#define START_ROM 512
#define STACK_SIZE 16

#define PIXEL_COLOR 0xFFFFFF
#define PIXEL_FUNC(color, bits, offset) ((color & bits) >> offset)

#endif
