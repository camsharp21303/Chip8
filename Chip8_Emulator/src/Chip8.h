/*
 * 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
 */

#ifndef CHIP8_H_
#define CHIP8_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <SDL2/SDL.h>

using namespace std;

class Chip8 {
private:
	SDL_Scancode SDLKeys[16] = {
			SDL_SCANCODE_0,
			SDL_SCANCODE_1,
			SDL_SCANCODE_2,
			SDL_SCANCODE_3,
			SDL_SCANCODE_4,
			SDL_SCANCODE_5,
			SDL_SCANCODE_6,
			SDL_SCANCODE_7,
			SDL_SCANCODE_8,
			SDL_SCANCODE_9,
			SDL_SCANCODE_A,
			SDL_SCANCODE_B,
			SDL_SCANCODE_C,
			SDL_SCANCODE_D,
			SDL_SCANCODE_E,
			SDL_SCANCODE_F,
	};
	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	unsigned short opcode; //2 byte opcodes
	unsigned char mem[4096]; //4KB memory
	unsigned char V[16]; //16 8bit registers
	unsigned short I; //2 byte index register
	unsigned short PC; //2 byte program counter

	//timers when set above zero, they count down to zero at 60hz
	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16]; //number of stack locations
	unsigned sp; //stack pointer

	unsigned char key[16]; //16 different keys 0-F

	void clearDisplay(){
		for(int x = 0; x < 64; x++){
			for(int y =0; y < 32; y++){
				dis[x][y] = 0;
			}
		}
	}
	void clear(unsigned char v[], int length){
		for(int i = 0; i < length; i++){
			v[i] = 0;
		}
	}
	void clear(unsigned short v[], int length){
		for(int i = 0; i < length; i++){
			v[i] = 0;
		}
	}
public:
	bool updateDisp;
	unsigned char dis[64][32]; //64x32 display array
	Chip8();
	void Load(string game);
	void Cycle(unsigned short op=0x0000);
	void updateKeys();
};

#endif /* CHIP8_H_ */
