#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "opcodes.h"
#include "chip8.h"

uint8_t font_char[80] = 
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

// Capitals OP_CLS --> enum instructions 
// Lowecse op_cls --> functions 

void stack_push(CHIP8_t *emu, uint16_t addr)
{   
    if(emu->stack.stack_ptr >= 15)
    {
        printf("Stack Overflow\n");
        return;
    }
    emu->stack.cell[++emu->stack.stack_ptr] = addr;
}

uint16_t stack_pop(CHIP8_t *emu)
{
    uint16_t ret_addr;

    if(emu->stack.stack_ptr < 0)
    {
        printf("Stack Underflow, no valued to pop\n");
        return 0x0000;
    }

    ret_addr = emu->stack.cell[emu->stack.stack_ptr--];
    return ret_addr;
}

void init(CHIP8_t *emu)
{
    // Loading the fonts into RAM
    for(int i = 0; i < 80; i++)
    {
        emu->RAM[0x50+i] = font_char[i]; 
    }

    for(int i =0; i<16; i++)
    {
        emu->gp_regs[i] = 0;
        emu->stack.cell[i] = 0;
    }

    emu->idx_regs = 0;
    emu->pc = 0x200;                            // Program starts at address 0x200
    emu->delay_timer = 0;
    emu->sound_timer = 0;
    emu->stack.stack_ptr = -1;

    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 64; j++)
        {
            emu->display_arr[i][j] = false;
        }
    }
    return;
}

int load_rom(CHIP8_t *emu, const char *filename)
{
    // open file in "rb" mode
    FILE *stream_ptr = fopen(filename, "rb");
    if(stream_ptr == NULL)
    {
        printf("\nError - File not found. returning -1\n");
        return -1;
    }

    // fseek to end, ftell to get size, fseek back to start
    int seek_status = fseek(stream_ptr, 0, SEEK_END);
    if(seek_status == -1)
    {
        printf("Error : Seek status failed, check your stream.\n");
        return -1;
    }
    int f_size = ftell(stream_ptr);
    seek_status = fseek(stream_ptr, 0, SEEK_SET);

    //check size <= 3584, bail if not
    if(f_size > 3584)
    {
        printf("Error : File is large and cannot fit the mem space.\n");
        return -1;
    }

    // frea the bytes in emu->RAM starting at offset 0x200
    // for(int i =0; i < f_size; i++)
    // {
    //     emu->RAM[0x200+i] = 0;
    // }

    int size_read = fread(&emu->RAM[0x200], 1, f_size, stream_ptr);
    if(size_read < f_size)
    {
        printf("Error: size read is less than given file_size\n");
        return -1;
    }
    // close the file
    int close_status = fclose(stream_ptr);
    if(close_status != 0)
    {
        return -1;
    }

    return 0;
}

uint16_t fetch_opcode(CHIP8_t *emu)
{
    uint16_t opcode;
    uint8_t msb = emu->RAM[emu->pc];
    uint8_t lsb = emu->RAM[emu->pc+1];

    emu->pc += 2;
    opcode = ( msb << 8 ) | lsb;        
    /*
        msb = 1110, lsb = 0110 
        msb << 8 -> 1110 0000 

        msb | lsb -> 1110 0000
                     0000 0110
                 or -----------
                     1110 0110
    
    */

    return opcode;
}

Instruction_t decode_opcode(uint16_t opcode)
{
    
    // get first nibble
    uint16_t mask = 0xF000; // 1111 0000 0000 0000
    uint16_t instruction_t = opcode & mask;
    instruction_t = instruction_t >> 12;

    switch (instruction_t)
    {
        case 0x0:
            switch (opcode & (0x00FF))
            {
                case 0x0E0:
                    return OP_CLS;
                case 0x0EE:
                    return OP_RET;
                default:
                    printf("Error: No opcode exist\n");
                    return OP_NOT_IMPLEMENTED;
            }
        case 0x1:
            return OP_JP_Addr;                  // 1nnn
        case 0x6:
            return OP_LOAD_BYTES;                // 6xkk
        case 0x7:
            return OP_ADD;                      // 7xnn
        case 0xA:
            return OP_LOAD_IDX;                 // ANNN
        case 0xD:
            return OP_DRW;                      // DXYN
        case 0x2:
            return OP_CALL_ADDR;                // 2NNN
        case 0x8:
            switch (opcode & 0x000F)
            {
                case 0x0:
                    return OP_LOAD_REGS;
                    break;
                case 0x1:
                    return OP_OR;               // 8xy1
                    break;
                case 0x2:
                    return OP_AND;               // 8xy2
                    break;
                case 0x3:
                    return OP_XOR;               // 8xy3
                    break;
                case 0x4:
                    return OP_REG_ADD;               // 8xy4
                    break;
                case 0x5:
                    return OP_SUB;               // 8xy5
                    break;
                case 0x6:
                    return OP_SHR;               // 8xy6
                    break;
                case 0x7:
                    return OP_SUBN;               // 8xy7
                    break;
                case 0xE:
                    return OP_SHL;               // 8xyE
                    break;
            }
            default:
            printf("OP CODE not added or do not exist\n");
            return OP_NOT_IMPLEMENTED;

    }
}

void execute_opcode(CHIP8_t *emu, uint16_t opcode, Instruction_t instruction)
{
    switch(instruction)
    {
        case OP_CLS:
            op_cls(emu);
            break;
        
        case OP_ADD:
            op_add(emu, opcode);
            break;

        case OP_DRW:
            op_draw(emu, opcode);
            break;

        case OP_JP_Addr:
            op_jump_addr(emu, opcode);
            break;

        case OP_LOAD_IDX:
            op_load_idx(emu, opcode);
            break;

        case OP_LOAD_BYTES:
            op_load_bytes(emu, opcode);
            break;

        case OP_NOT_IMPLEMENTED:
            printf("Opcode not implemented or invalid\n");
            break;

        case OP_CALL_ADDR:
            op_call_addr(emu, opcode);
            break;
        case OP_RET:
            op_ret(emu);
            break;

        case OP_LOAD_REGS:
            op_load_regs(emu, opcode);
            break;
            
        case OP_OR:
            op_or(emu);
            break;
        case OP_AND:
            op_and(emu);
            break;
        case OP_XOR:
            op_xor(emu);
            break;
        case OP_REG_ADD:
            op_reg_add(emu);
            break;
        // case OP_SUB:
        //     op_ret(emu);
        //     break;
        // case OP_SHR:
        //     op_ret(emu);
        //     break;
        // case OP_SUBN:
        //     op_ret(emu);
        //     break;
        // case OP_SHL:
        //     op_ret(emu);
        //     break;
    }
}

void render_display(CHIP8_t *emu)
{
    for (int i = 0; i < 32; i++)          // for each row
    {
        for (int j = 0; j < 64; j++)      // each column in that row
        {
            if (emu->display_arr[i][j] == true)
            {
                printf("█");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");                     // move to next line after finishing a row
    }
}

int main(int argc, char **argv)
{
    CHIP8_t emu;
    init(&emu);

    if(argc <= 1)
    {
        printf("argc test failed\n");
        return -1;
    }
    else if(argc > 1)
    {   
        printf("argc test passed.\n");
        if( load_rom(&emu, argv[1]) == -1)
        {
            printf("loading ROM failed\n");
            return -1;
        }
        printf("load test passed\n");
    }   
    
    for(int i = 0; i < 30; i++)
    {
        uint16_t opcode = fetch_opcode(&emu);
        Instruction_t instruction = decode_opcode(opcode);
        execute_opcode(&emu, opcode, instruction);
    }


    render_display(&emu);
    exit(EXIT_SUCCESS);
}
