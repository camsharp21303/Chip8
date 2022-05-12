#include <chip8_cpu.h>

chip8_cpu::chip8_cpu(ifstream& rom_file){
    for(int i = 0; i < MEMORY_SIZE; i++){
        RAM[i] = 0;
    }

    for(int i = 0; i < 16; i++){
        V_reg[i] = 0;
        key[i] = 0;
    }

    I = 0;
    PC = START_ROM;

    for(int i = 0; i < STACK_SIZE; i++){
        stack[i] = 0;
    }
    sp = 0;

    timer_delay = 0;
    timer_sound = 0;

    update_disp = false;

    //copy fontset into memory
    memcpy(RAM, fontset, 80);


    //find size of our rom
    int size = rom_file.tellg();
    rom_file.seekg(0, std::ios::end);
    size = (int)rom_file.tellg() - size;
    rom_file.seekg(std::ios::beg);

    char* buffer = new char[size];

    //read rom into buffer
    rom_file.read(buffer, size);
    memcpy(&RAM[START_ROM], buffer, size);

    delete[] buffer;
    rom_file.close();

}

void chip8_cpu::cycle(){
    short opcode = RAM[PC] << 8 | RAM[PC+1];
    update_disp = false;

    unsigned char x, y;

    switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x000F){
                case 0x000E: //return from subroutine
                    sp--;
                    PC = stack[sp];
                    break;
                case 0x0000: //clearDisplay
                    //TODO clear Display
                    break;
            }
            break;
        case 0x1000:// jump to address 0x1NNN
            PC = opcode & 0x0FFF;
            PC -= 2; //to cancel out move forward at end of cycle
            break;

        case 0x2000: //call subroutine at 0x2NNN
            stack[sp++] = PC;
            PC = opcode & 0x0FFF;
            PC -= 2;
            break;

        case 0x3000: //skip next instruction if VX EQUAL NN 0x3XNN
            if(V_reg[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) PC += 2;
            break;

        case 0x4000: //0x4XNN, skip next i nstruction if VX != NN
            if(V_reg[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) PC += 2;
            break;

        case 0x5000: //0x5XY0, skip next instruction if VX == VY
            if(V_reg[(opcode & 0x0F00) >> 8] == V_reg[(opcode & 0x00F0) >> 4]) PC += 2;
            break;

        case 0x6000: //0x6XNN, set VX to N
            V_reg[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;

        case 0x7000: //0x7XNN, add NN to VX
            V_reg[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;

        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000: //0x8XY0, sets VX to VY
                    V_reg[(opcode & 0x0F00) >> 8] = V_reg[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0001: //0x8XY1, sets VX to VX | VY
                    V_reg[(opcode & 0x0F00) >> 8] |= V_reg[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0002: //0x8XY2, sets VX to VX and VY
                    V_reg[(opcode & 0x0F00) >> 8] &= V_reg[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0003: //0x8XY3, sets VX to VX xor VY
                    V_reg[(opcode & 0x0F00) >> 8] ^= V_reg[(opcode & 0x00F0) >> 4];
                    break;
                case 0x0004: //0x8XY4, VX += VY, VF set if carry
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    if(V_reg[y] > (0xFF - V_reg[x])) V_reg[0xF] = 1;
                    else V_reg[0xF] = 0;
                    V_reg[x] += V_reg[y];
                    break;
                case 0x0005: //0x8XY5, VX -= VY, VF set to 0 if borrow
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    if(V_reg[y] > V_reg[x]) V_reg[0xF] = 0;
                    else V_reg[0xF] = 1;
                    V_reg[x] -= V_reg[y];
                    break;
                case 0x0006: //0x8XY6, stores LSB of VX in VF then srl VX
                    x = (opcode & 0x0F00) >> 8;
                    V_reg[0xF] = V_reg[x] & 0x1;
                    V_reg[x] >>= 1;
                    break;
                case 0x0007: //0x8XY7, VX = VY - VX, VF set to 0 when borrow
                    x = (opcode & 0x0F00) >> 8;
                    y = (opcode & 0x00F0) >> 4;
                    if(V_reg[x] > V_reg[y]) V_reg[0xF] = 0;
                    else V_reg[0xF] = 1;
                    V_reg[x] = V_reg[y] - V_reg[x];
                    break;
                case 0x000E: //0x8XYE, stores MSB of VX in VF, sll VX
                    x = (opcode & 0x0F00) >> 8;
                    V_reg[0xF] = V_reg[x] & 0x8000;
                    V_reg[x] <<= 1;
            }
            break;

        case 0x9000: //0x9XY0, skip next instcruction if VX does not equal VY
            x = (opcode & 0x0F00) >> 8;
            y = (opcode & 0x00F0) >> 4;
            if(V_reg[x] != V_reg[y]) PC += 2;
            break;

        case 0xA000: //0xANNN, sets I to address NNN
            I = (opcode & 0x0FFF);
            break;

        case 0xB000: //0xBNNN, jusmps to address NNN + V0
            PC += (opcode & 0x0FFF) + V_reg[0] - 2;
            break;

        case 0xC000: //0xCXNN, sets VX to the result of a bitwise and operation on a random number (0 to 255) and NN
            V_reg[(opcode & 0x0F00) >> 8] = (rand() & 255) & (opcode & 0x00FF);
            break;

        case 0xD000: //TODO display

        case 0xE000: //TOD keyboard input

        case 0xF000: //TODO memory stoof
            break;
    }

    PC += 2;
}