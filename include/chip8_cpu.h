#ifndef CHIP8_CPU
#define CHIP8_CPU

#include <defns.h>
#include <fstream>
using std::ifstream;

class chip8_cpu{
    private:
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