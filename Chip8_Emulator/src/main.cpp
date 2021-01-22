#include <SDL2/SDL.h>
#include <iostream>
#include "GfxProc.h"
#include "Chip8.h"
using namespace std;


int scale = 10;					//Scale of the game window
int WINDOW_WIDTH = 64*scale;	//game window scale
int WINDOW_HEIGHT = 32*scale;

int main (int argc, char* args[] )
{
	SDL_Event e;
	GfxProc gfx(WINDOW_WIDTH, WINDOW_HEIGHT, scale); //create game window
	bool quit = false;
	Chip8* cpu = new Chip8();

	if(argc < 2){
		cout << "No Rom specified" << endl;
		return 1;
	}
	cpu->Load(args[1]);	//load game into chip 8 cpu

	//start execution loop, check if user wants to quit
	while(!quit){
		//checks if user is pressing quit key
		while( SDL_PollEvent( &e ) != 0 ){
			if( e.type == SDL_QUIT ) //set quit to true
			{
				quit = true;
			}
		}

		//execute one cpu cycle
		//also updates key registers from SDL
		cpu->updateKeys();
		cpu->Cycle();
		if(cpu->updateDisp){
			gfx.renderSurface(cpu->dis);
		}

		//small delay to help reduce frametimes, my computers just too good
		//TODO change to conistent clock speed
		SDL_Delay(1);
	}
    return 0;
}


