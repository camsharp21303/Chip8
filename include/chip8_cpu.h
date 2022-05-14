#ifndef CHIP8_CPU
#define CHIP8_CPU

#include <defns.h>
#include <fstream>
#include <iostream>
using std::ifstream;

class chip8_cpu{
    private:
        unsigned char emu_fontset[80] =
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
        //stores one byte memory locations
        unsigned char RAM[MEMORY_SIZE];

        //our registers, 16 1 byte regs, and 2 2byte regs
        unsigned char V_reg[16];
        unsigned short I;
        unsigned short PC;

        //store our stack
        unsigned char stack[STACK_SIZE];
        unsigned char sp;

        //two timers, count down at 60hz until they reach zero
        unsigned char timer_delay;
        unsigned char timer_sound;

        //our keyboard
        unsigned char key[16];

        bool update_disp;

    public:
        //our display
        unsigned char display[64][32];

        chip8_cpu(ifstream& rom_file);
        void cycle();
        bool display_check();

};

#endif
