#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>

typedef struct
{
    uint16_t cell[16];
    uint8_t stack_ptr;

} stack_t;

typedef struct
{
    uint8_t RAM[4096];
    uint8_t gp_regs[16];
    uint16_t idx_regs;
    uint16_t pc;
    uint8_t delay_timer, sound_timer;
    stack_t stack;
    bool display_arr[32][64];

} CHIP8_t;

void init(CHIP8_t *emu);

#endif