#ifndef OPCODES_H
#define OPCODES_H

#include "chip8.h"

void op_load_regs(CHIP8_t *emu, uint16_t opcode);
void op_jump_addr(CHIP8_t *emu, uint16_t opcode);
void op_add(CHIP8_t *emu, uint16_t opcode);
void op_load_idx(CHIP8_t *emu, uint16_t opcode);
void op_cls(CHIP8_t *emu);
void op_draw(CHIP8_t *emu, uint16_t opcode);


#endif