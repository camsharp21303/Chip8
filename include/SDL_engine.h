#ifndef SDL_ENGINE_H
#define SDL_ENGINE_H

#include <iostream>
#include <SDL2/SDL.h>
#include <defns.h>

namespace std{
    graphics_SDL* engine_Start(const char* title);
    void engine_Stop(graphics_SDL* engine);

    void engine_UpdateSurface(graphics_SDL* engine);

    void chip8_display(graphics_SDL* engine, const unsigned char display_bits[64][32]);

    bool is_quit();
}

#endif
