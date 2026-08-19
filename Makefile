CFLAGS = -Wall -Wextra -Werror

all:
	gcc chip8.c -o chip8 $(CFLAGS) 