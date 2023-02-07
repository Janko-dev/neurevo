CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g

IN = la.c main.c
OUT = out

make: $(IN)
	$(CC) $(IN) -o $(OUT) $(CFLAGS)