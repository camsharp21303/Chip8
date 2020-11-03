#include "Chip8.h"
#include <iostream>

Chip8::Chip8() {
	PC = 0x200;
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
	long size;
	_file = fopen(game.c_str(), "rb");
	if(_file == NULL){
		fputs ("File error",stderr); exit (1);
	}
	fseek(_file, 0, SEEK_END);
	size = ftell(_file);
	rewind(_file);

	char* buffer = (char*) malloc(sizeof(char)*size);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	fread(buffer, 1, size, _file);

	for(int i = 0; i < size; i++){
		mem[i+512] = buffer[i];
	}

	fclose(_file);
}

void Chip8::updateKeys(){
	clear(key, 16);
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	//SDL_PumpEvents();

	for(int i = 0; i < 16; i++){
		if(keystates[SDLKeys[i]]) key[i] = 1;
		else key[i] = 0;
	}
}

void Chip8::Cycle(unsigned short op){
	bool diagnose = true;
	if(op == 0x0000){
		opcode = mem[PC] << 8 | mem[PC+1];
		diagnose = false;
	}
	else opcode = op;
	updateDisp = false;

	//cout << hex << opcode << "       " << PC << endl;
	switch(opcode & 0xF000){
		case 0xA000:
			//good
			I = opcode & 0x0FFF;
			PC += 2;
			break;
		case 0x0000:
			switch(opcode & 0x000F){
				case 0x0000:
					//good
					clearDisplay();
					updateDisp = true;
					PC += 2;
					break;
				case 0x000E:
					sp--;
					PC = stack[sp];
					PC+=2;
					break;
			}
			break;
		case 0x1000:
			//good
			PC = opcode & 0x0FFF;
			break;
		case 0x2000:
			stack[sp] = PC;
			sp++;
			PC = opcode & 0x0FFF;
			break;
		case 0x3000:
			//good
			if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
				PC+=4;
			}
			else{
				PC+=2;
			}
			break;
		case 0x4000:
			//good
			if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
				PC+=4;
			}
			else{
				PC+=2;
			}
			break;
		case 0x5000:
			//good
			if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
				PC += 4;
			}
			else{
				PC +=2;
			}
			break;
		case 0x6000:
			//good
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			PC += 2;
			break;
		case 0x7000:
			//good
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			PC += 2;
			break;
		case 0x8000:
			switch(opcode & 0x000F){
				case 0x0000:
					//good
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0001:
					//good
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0002:
					//good
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0003:
					//good
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0004:
					//good
					if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) V[0xF] = 1;
					else V[0xF] = 0;
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0005:
					//good
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) V[0xF] = 0;
					else V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					break;
				case 0x0006:
					//good
					V[0xF] = V[(opcode & 0x0F00) >> 8] % 2;
					V[(opcode & 0x0F00) >> 8] >>=1;
					break;
				case 0x0007:
					if(V[(opcode & 0x00F0) >> 4] < (V[(opcode & 0x0F00) >> 8])) V[0xF] = 0;
					else V[0xF] = 1;
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					break;
				case 0x000E:
					//good
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<=1;
					break;
			}
			PC+=2;
			break;
			case 0x9000:
				//good
				if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) PC+=4;
				else PC+=2;
				break;
			case 0xB000:
				PC = (opcode & 0x0FFF) + V[0x0];
				break;
			case 0xC000:
				//good
				V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
				PC+=2;
				break;
			case 0xD000:{
				//good
				unsigned char x = V[(opcode & 0x0F00) >> 8];
				unsigned char y = V[(opcode & 0x00F0) >> 4];
				unsigned char rows = (opcode & 0x000F);
				V[0xF] = 0;
				for(int i = 0; i < rows; i++){
					y++;
					unsigned char data = mem[I+i];
					for(int j = 3; j >= 0; j--){
						unsigned int pix = (((data >> 4) & (unsigned char)pow(2,j)) >> j);
						unsigned char o = 3-j;
						if(dis[o+x][y] != 0){
							V[0xF] = 1;
						}
						dis[o+x][y] ^= pix;
					}
				}
				updateDisp = true;
				PC+=2;
				break;}
			case 0xE000:
				switch(opcode & 0x00F0){
					case 0x0090:
						if(key[V[(opcode & 0x0F00) >> 8]] > 0) PC+=4;
						else PC+=2;
						break;
					case 0x00A0:
						if(key[V[(opcode & 0x0F00) >> 8]] == 1) PC+=2;
						else PC+=4;
						break;
				}
				break;
			case 0xF000:
				switch (opcode & 0x000F){
					case 0x0007:
						//good
						V[(opcode & 0x0F00) >> 8] = delay_timer;
						PC += 2;
						break;
					case 0x000A:
						for(int i = 0; i < 16; i++){
							if(key[i] > 0){
								PC+=2;
								V[(opcode * 0x0F00) >> 8] = i;
								break;
							}
						}
						PC += 2;
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
						PC += 2;
						break;
					case 0x0008:
						sound_timer = V[(opcode & 0x0F00) >> 8];
						PC += 2;
						break;
					case 0x000E:
						//good
						I += V[(opcode & 0x0F00) >> 8];
						PC += 2;
						break;
					case 0x0009:{
						unsigned char digit = V[(opcode & 0x0F00) >> 8];
						I = digit*5;
						PC += 2;
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
	if(sound_timer == 0) cout << "sound";
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

