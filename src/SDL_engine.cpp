#include <SDL_engine.h>

graphics_SDL* std::engine_Start(const char* title){
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cout << "SDL could not init, " << SDL_GetError() << endl;
        return NULL;
    }

    //create the window
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL){
        cout << "Window could not be created" << SDL_GetError() << endl;
        return NULL;
    }

    //get surface to update from window
    screenSurface = SDL_GetWindowSurface(window);

    graphics_SDL* newEngine = new graphics_SDL;
    newEngine->window = window;
    newEngine->surface = screenSurface;

    return newEngine;

}
void std::engine_Stop(graphics_SDL* engine){
    SDL_DestroyWindow(engine->window);
    SDL_Quit();

    delete engine;
}

void std::engine_UpdateSurface(graphics_SDL* engine){
    SDL_UpdateWindowSurface(engine->window);
}

bool std::is_quit(){
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            return true;
        }
    }

    return false;
}

void std::chip8_display(graphics_SDL* engine, const unsigned char display_bits[64][32]){
  for(int i = 0; i < 64; i++){
    for(int j = 0; j < 32; j++){
        SDL_Rect rectangle;
        rectangle.x = i * MULTIPLIER(WINDOW_WIDTH, 64);
        rectangle.y = j * MULTIPLIER(WINDOW_HEIGHT, 32);
        rectangle.w = MULTIPLIER(WINDOW_WIDTH, 64);
        rectangle.h = MULTIPLIER(WINDOW_HEIGHT, 32);
        unsigned int color;
        if(display_bits[i][j] == 0){
          color = SDL_MapRGB(engine->surface->format, 0, 0, 0);
        }
        else{
          color = SDL_MapRGB(
            engine->surface->format,
            PIXEL_FUNC(PIXEL_COLOR, 0xFF0000, 16),
            PIXEL_FUNC(PIXEL_COLOR, 0x00FF00, 8),
            PIXEL_FUNC(PIXEL_COLOR, 0x0000FF, 0)
          );
        }
        SDL_FillRect(engine->surface, &rectangle, color);
      }
    }
}
