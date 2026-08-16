#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

} CHIP8_t;

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
    emu->stack.stack_ptr = 0;
}

int load_rom(CHIP8_t *emu, const char *filename)
{
    // open file in "rb" mode
    FILE *stream_ptr = fopen(filename, "rb");
    if(stream_ptr == NULL)
    {
        printf("\nError - File not found. returning -1");
        return -1;
    }

    // fseek to end, ftell to get size, fseek back to start
    int seek_status = fseek(stream_ptr, 0, SEEK_END);
    if(seek_status == -1)
    {
        printf("Error : Seek status failed, check your stream.");
        return -1;
    }
    int f_size = ftell(stream_ptr);
    seek_status = fseek(stream_ptr, 0, SEEK_SET);

    //check size <= 3584, bail if not
    if(f_size > 3584)
    {
        printf("Error : File is large and cannot fit the mem space.");
        return -1;
    }

    // frea the bytes in emu->RAM starting at offset 0x200
    // for(int i =0; i < f_size; i++)
    // {
    //     emu->RAM[0x200+i] = 0;
    // }

    int items = fread(&emu->RAM[0x200], 1, f_size, stream_ptr);

    // close the file
    int close_status = fclose(stream_ptr);
    if(close_status != 0)
    {
        return -1;
    }
}


int main(int argc, char **argv)
{
    CHIP8_t emu;
    init(&emu);

    exit(EXIT_SUCCESS);
}