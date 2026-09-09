#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void write_rom_test()
{
    FILE* fptr = fopen("Usuables/rom_test.ch8", "wb");       // write binary
    unsigned char rom[] = {0x22,0x06,0x12,0x04,0x12,0x04,0x60,0x05,0x00,0xEE};
    fwrite(rom, 1, sizeof(rom), fptr);
    fclose(fptr);
}


int main()
{
    write_rom_test();
    return 0;
}