#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>

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

typedef enum Instruction_t {OP_CLS, OP_JP_Addr, OP_LOAD_REGS, OP_LOAD_IDX, OP_DRW, OP_ADD, OP_NOT_IMPLEMENTED} Instruction_t;

void init(CHIP8_t *emu);

#endif