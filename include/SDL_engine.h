#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <defns.h>

namespace std{
    graphics_SDL* engine_Start(const char* title);
    void engine_Stop(graphics_SDL* engine);

    void engine_UpdateSurface(graphics_SDL* engine);

    bool is_quit();
}