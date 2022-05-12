#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <chip8_cpu.h>
#include <defns.h>
#include <SDL_engine.h>
using namespace std;

int main(int argc, char** argv){
    if(argc < 2){
        cout << "Must specify a ROM to load!" << endl;
        return -1;
    }
    ifstream rom;
    rom.open(argv[1], ios::binary);
    if(!rom.is_open()){
        cout << "Failed to Open ROM!" << endl;
        return -1;
    }

    chip8_cpu cpu(rom);


    graphics_SDL* engine = engine_Start("Chip 8 Emulator");

    //fill surface with white
    SDL_FillRect(engine->surface, NULL, SDL_MapRGB(engine->surface->format, 0x00, 0xFF, 0xFF));


    bool quit = false;
    while(!quit){
        //update the surface
        engine_UpdateSurface(engine);

        quit = is_quit();
    }

    engine_Stop(engine);

    return 0;
}