CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

prog: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
