CC=gcc
ARGS=-g -Wall -Wextra -Wshadow  -lmingw32 -lSDL2main -lSDL2
# to get rid of console: -mwindows

all: main

main: main.o logic.o render.o
	$(CC) main.o logic.o render.o -o main $(ARGS) 

%.o: %.c
	$(CC) -c $^ $(ARGS) 

clean:
	rm main.exe *.o