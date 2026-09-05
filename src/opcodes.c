#include <stdio.h>
#include "chip8.h"
#include <stdlib.h>
#include <stdint.h>

void op_load_bytes(CHIP8_t *emu, uint16_t opcode)
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

void op_jump_addr(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t address = opcode & 0x0FFF;
    // No bound check needed, it is solved by masking. also let programs access whole 4KB, do not restrict 0x000 - 0x200
    emu->pc = address;
    return;
}

void op_add(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t reg_num = (opcode & 0x0F00) >> 8;
    uint8_t operands = opcode & 0x00FF;

    emu->gp_regs[reg_num] += operands;

    return;
}

void op_load_idx(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t idx = opcode & 0x0FFF;
    emu->idx_regs = idx;
    return;
}

void op_cls(CHIP8_t *emu)
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
void op_draw(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;

    uint8_t x_start = emu->gp_regs[X];           // starting x coordinate (origin)
    uint8_t y_start = emu->gp_regs[Y];           // starting y coordinate (origin)

    uint8_t object_row;

    for(int i = 0; i < N; i++)
    {
        object_row = emu->RAM[emu->idx_regs+i];
        for(int j =0; j<8; j++)
        {
            uint8_t sprite_pixel = (object_row >> (7-j)) & 0x1;     // diary note 29 APRIL

            uint8_t screen_pos_x = x_start + j;
            uint8_t screen_pos_y = y_start + i;

            if (sprite_pixel == 1)
            {
                emu->display_arr[screen_pos_y][screen_pos_x] ^= true;
            }
        }
    }
    return;
}

void op_call_addr(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t address = (opcode & 0x0FFF);
    uint16_t current_pc = emu->pc;
    stack_push(emu, current_pc);

    emu->pc = address;
}

void op_ret(CHIP8_t *emu)
{
    emu->pc = stack_pop(emu);
}

void op_load_regs(CHIP8_t *emu, uint16_t opcode)
{
    uint8_t Vy = emu->gp_regs[(opcode & 0x00F0) >> 4];
    emu->gp_regs[(opcode & 0x0F00) >> 8] = Vy;
}

void op_or(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t Vx = (opcode & 0x0F00) >> 8;
    uint16_t Vy = (opcode & 0x00F0) >> 4;
    emu->gp_regs[Vx] = emu->gp_regs[Vx] | emu->gp_regs[Vy];
}

void op_and(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t Vx = (opcode & 0x0F00) >> 8;
    uint16_t Vy = (opcode & 0x00F0) >> 4;
    emu->gp_regs[Vx] = emu->gp_regs[Vx] & emu->gp_regs[Vy];
}

void op_xor(CHIP8_t *emu, uint16_t opcode)
{
    uint16_t Vx = (opcode & 0x0F00) >> 8;
    uint16_t Vy = (opcode & 0x00F0) >> 4;
    emu->gp_regs[Vx] = emu->gp_regs[Vx] ^ emu->gp_regs[Vy];
}
