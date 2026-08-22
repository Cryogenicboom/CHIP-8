#include <stdio.h>
#include "chip8.h"
#include <stdlib.h>
#include <stdint.h>

void OP_LOAD_REGS(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t reg_number = (opcode & 0x0F00 ) >> 8;
    if(reg_number >= 16)
    {
        printf("Error: Invlid Registers used. GP register limit exceed.\n");
        return;
    }
    emu->gp_regs[reg_number] = opcode & 0x00FF;
    return;
}

void OP_JUMP_ADDR(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t address = opcode & 0x0FFF;
    // No bound check needed, it is solved by masking. also let programs access whole 4KB, do not restrict 0x000 - 0x200
    emu->pc = address;
    return;
}

void OP_ADD(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t reg_num = (opcode & 0x0F00) >> 8;
    uint8_t operands = opcode & 0x00FF;

    emu->gp_regs[reg_num] += operands;

    return;
}

void OP_LOAD_IDX(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t idx = opcode & 0x0FFF;
    emu->idx_regs = idx;
    return;
}

void OP_CLS(CHIP8_t *emu)
{
    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            emu->display_arr[i][j] = false;
        }
    }
    return;
}

//DXYN
void OP_DRW(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;

    uint8_t x_codn = emu->gp_regs[X];
    uint8_t y_codn = emu->gp_regs[Y];

    uint8_t object_row;

    for(int i = 0; i < N; i++)
    {
        object_row = emu->RAM[emu->idx_regs+i];
    }

}

