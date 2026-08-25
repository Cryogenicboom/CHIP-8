CFLAGS = -g -Wall -Werror
CC = gcc

FILES = src/chip8.c \
src/opcodes.c \
-Isrc/include

OUT = CHIP8

all:
	$(CC) $(CFLAGS) $(FILES) -o $(OUT)

run: 
	./$(OUT)

clean:
	rm -f $(OUT)