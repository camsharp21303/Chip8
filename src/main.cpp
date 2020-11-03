#include <SDL2/SDL.h>
#include <iostream>
#include "GfxProc.h"
#include "Chip8.h"
using namespace std;

int scale = 10;
int WINDOW_WIDTH = 64*scale;
int WINDOW_HEIGHT = 32*scale;

int main (int argc, char* args[] )
{
	SDL_Event e;
	GfxProc gfx(WINDOW_WIDTH, WINDOW_HEIGHT, scale);
	bool quit = false;
	Chip8* cpu = new Chip8();
	cpu->Load("C:\\Users\\camer\\eclipse-workspace\\Chip-8\\Debug\\PONG");

	while(!quit){
		unsigned short op;
		while( SDL_PollEvent( &e ) != 0 ){
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}

		/*cout << "Input opcode: ";
		cin >> hex >> op;
		cout << endl;*/


		cpu->updateKeys();
		cpu->Cycle();
		if(cpu->updateDisp){
			gfx.renderSurface(cpu->dis);
		}
		//cout << "a";
		SDL_Delay(1);
	}

    return 0;
}


