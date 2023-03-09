CC = gcc
CFLAGS = -Wall -g -Wextra -std=c11
SRC = src/
IN = $(SRC)nn.c $(SRC)texture.c $(SRC)graphics.c $(SRC)main.c
OUT = nevo

ifeq ($(OS), Windows_NT)
	SDL = -IC:\Users\janba\Dev\i686-w64-mingw32\include -LC:\Users\janba\Dev\i686-w64-mingw32\lib -lmingw32 -lSDL2main -lSDL2
else 
	SDL = `pkg-config --cflags --libs sdl2`
endif

default:
	$(CC) $(IN) -o $(OUT) $(CFLAGS) $(SDL)