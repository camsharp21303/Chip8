#include "Chip8.h"
#include <iostream>


//This is the fun stuff

Chip8::Chip8() {
	PC = 0x200; //start counter at where the program starts
	opcode = 0;
	I = 0;
	sp = 0;
	clearDisplay();
	clear(stack, 16); //clear stack
	clear(mem, 4096); //clear memory
	clear(V, 16); //clear registers

	//init fontset
	for(int i = 0; i < 80; i++){
		mem[i] = chip8_fontset[i];
	}

	delay_timer = 0;
	sound_timer = 0;
	updateDisp = false;
}

void Chip8::Load(string game){
	FILE * _file;
	long size; //stores size of file in bytes
	_file = fopen(game.c_str(), "rb");
	if(_file == NULL){
		fputs ("File error",stderr); exit (1);
	}
	//go to end of file
	fseek(_file, 0, SEEK_END);
	//report size of file
	size = ftell(_file);
	//go back to beginnig of file
	rewind(_file);

	//create a buffer that stores the rom for Chip8
	char* buffer = (char*) malloc(sizeof(char)*size);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	//store ROM into buffer
	fread(buffer, 1, size, _file);

	//now store the buffer into Chip8 memory
	for(int i = 0; i < size; i++){
		mem[i+512] = buffer[i];
	}

	//close the file
	fclose(_file);
}

void Chip8::updateKeys(){
	clear(key, 16);
	const Uint8* keystates = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < 16; i++){
		if(keystates[SDLKeys[i]]) key[i] = 1;
		else key[i] = 0;
	}
}

void Chip8::Cycle(unsigned short op){
	//defines whether to add telemetry or not
	bool diagnose = true;

	//if user does not start at opcode
	//for debugging
	//TODO delete
	if(op == 0x0000){
		opcode = mem[PC] << 8 | mem[PC+1];
		diagnose = false;
	}
	else opcode = op;

	//by default don't update display
	updateDisp = false;

	//this is how we process our opcodes
	switch(opcode & 0xF000){
		case 0xA000: //0xANNN
			//Sets the Index to the address of NNN
			I = opcode & 0x0FFF;
			break;

		case 0x0000:
			switch(opcode & 0x000F){
				case 0x0000: //0x00E0
					//clears the display
					clearDisplay();
					updateDisp = true;
					break;
				case 0x000E: //0x00EE
					//return from subroutine
					sp--;
					PC = stack[sp];
					break;
			}
			break;

		case 0x1000: //0x1NNN
			//Jump to address NNN
			PC = opcode & 0x0FFF;
			PC -= 2;
			break;

		case 0x2000: //2NNN
			//Call subroutine at NNN
			stack[sp] = PC;
			sp++;
			PC = opcode & 0x0FFF;
			PC -= 2;
			break;

		case 0x3000: //3XNN
			//Skip next instruction
			//if VX equal NN
			if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) PC+=2;
			break;

		case 0x4000: //4XNN
			//Skip next instruction
			//if VX does not equal NN
			if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) PC+=2;
			break;

		case 0x5000: //5XY0
			//Skip next insturction
			//if VX equals VY
			if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) PC += 2;
			break;

		case 0x6000: //6XNN
			//Sets VX to NN
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			break;

		case 0x7000: //7XNN
			//Adds NN to VX
			//Carry flag is unaffected, yay, easy
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			break;

		case 0x8000: //Primarily involves bitwise operations (learned a lot on this one)
			switch(opcode & 0x000F){
				case 0x0000: //8XY0
					//Sets VX to the value of VY
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0001: //8XY1
					//Sets VX to (VX or VY) in bitwise
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0002: //8XY2
					//Sets VX to (VX and VY) in bitwise
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0003: //8XY3
					//Sets VX to (VX xor VY) in bitwise
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0004: //8XY4
					//Adds VY to VX
					//If there is a carry VF is set to 1, otherwise 0
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) V[0xF] = 1;
					else V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0005: //8XY5
					//VY is subtracted from VX
					//VF set to 0 when there is a borrow, otherwise 1
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) V[0xF] = 0;
					else V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0006: //8XY6
					/*
					 * Stores the right most bit of VX into VF
					 * Then shifts VX over to the right by one
					 * good for checking for even or odd
					 * also good for dividing by 2
					 */

					V[0xF] = V[(opcode & 0x0F00) >> 8] % 2;
					V[(opcode & 0x0F00) >> 8] >>=1;
					break;
				case 0x0007: //8XY7
					//Sets VX to (VY-VX)
					//VF set to zero when borrow, otherwise 1
					if(V[(opcode & 0x00F0) >> 4] < (V[(opcode & 0x0F00) >> 8])) V[0xF] = 0;
					else V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					break;
				case 0x000E: //8XYE
					/*
					 * Stores most left bit into VF
					 * Then shifts VX to the left by one bit
					 * good for multiplying by 2 (unless it is odd)
					 */
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<=1;
					break;
			}
			break;

			case 0x9000: //9XY0
				//Skips next instruction if VX does not equal VY
				if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) PC+=2;
				break;

			case 0xB000: //BNNN
				//Jump to address NNN plus V0
				PC = (opcode & 0x0FFF) + V[0x0];
				PC -= 2;
				break;

			case 0xC000: //CXNN
				//VX is set to random number from 0 to 255
				//Which is and'ed with NN
				V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
				break;

			case 0xD000:{ //DXYN
				/*
				 * OH boy, this ones fun
				 * This draws a sprite at coordinate (VX, VY)
				 * Each sprite has a width of 8 pixels
				 * And a height of N+1
				 * Each row is read from memory location I, although I never changes
				 * If any pixel is already set, and gets set or again , then set VF to 1
				 * 		This is useful for collision detection, otherwise set to 0
				 */

				//First find x coordinate
				unsigned char x = V[(opcode & 0x0F00) >> 8];
				unsigned char y = V[(opcode & 0x00F0) >> 4]; //find Y coordinate
				unsigned char rows = (opcode & 0x000F); //Find the number of rows
				V[0xF] = 0; //colllision set to 0
				for(int i = 0; i < rows; i++){
					unsigned char data = mem[I+i];
					for(int j = 3; j >= 0; j--){
						unsigned int pix = (((data >> 4) & (unsigned char)pow(2,j)) >> j);
						unsigned char o = 3-j;
						if(dis[o+x][y] != 0){
							V[0xF] = 1;
						}
						dis[o+x][y] ^= pix;
					}
					y++;
				}
				updateDisp = true;
				break;}
			case 0xE000:
				switch(opcode & 0x00F0){
					case 0x0090:
						if(key[V[(opcode & 0x0F00) >> 8]] > 0) PC+=2;
						break;
					case 0x00A0:
						if(key[V[(opcode & 0x0F00) >> 8]] != 1) PC+=2;
						break;
				}
				break;
			case 0xF000:
				switch (opcode & 0x000F){
					case 0x0007:
						//good
						V[(opcode & 0x0F00) >> 8] = delay_timer;
						break;
					case 0x000A:
						for(int i = 0; i < 16; i++){
							if(key[i] > 0){
								PC+=2;
								V[(opcode * 0x0F00) >> 8] = i;
								break;
							}
						}
						break;
					case 0x0005:
						switch(opcode & 0x00F0){
							case 0x0010:
								//good
								delay_timer = V[(opcode & 0x0F00) >> 8];
								break;
							case 0x0050:{
								unsigned char index = (opcode & 0x0F00) >> 8;
								for(int i = 0; i <= index; i++){
									mem[I + i] = V[i];
								}
								break;}
							case 0x0060:{
								unsigned char index = (opcode & 0x0F00) >> 8;
								for(int i = 0; i <= index; i++){
									V[i] = mem[I + i];
								}
								break;}
						}
						break;
					case 0x0008:
						sound_timer = V[(opcode & 0x0F00) >> 8];
						break;
					case 0x000E:
						//good
						I += V[(opcode & 0x0F00) >> 8];
						break;
					case 0x0009:{
						unsigned char digit = V[(opcode & 0x0F00) >> 8];
						I = digit*5;
						break;}
					case 0x0003:{
						unsigned char d = V[(opcode & 0x0F00) >> 8];
						int hundreds = d/100;
						int tens = (d%100)/10;
						int ones = (d%100)%10;
						mem[I] = hundreds;
						mem[I+1] = tens;
						mem[I+2] = ones;
						PC += 2;
						break;}
				}
				break;
	}
	if(delay_timer > 0){
		delay_timer--;
	}
	if(sound_timer > 0){
		sound_timer--;
	}

	PC += 2;

	if(diagnose){
		system("CLS");
		cout << flush;
		cout << "Diagnostic Info" << endl;
		cout << "Mem Index: " << I << endl;
		cout << endl << "Registrars" << endl;
		for(int i = 0; i < 16; i++){
			cout << "V[" << i << "]: " << dec << (unsigned short)V[i] << endl << flush;
		}
		cout << endl << "PC: " << dec << PC << endl;
		cout << "Delay: " << delay_timer << endl;
		cout << "Sound: " << sound_timer << endl;
	}
}

