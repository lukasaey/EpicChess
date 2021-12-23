CC=gcc
DEBUGARGS=-g -Wall -Wextra -Wshadow -lmingw32 -lSDL2main -lSDL2
RELEASEARGS=-O2 -mwindows -lmingw32 -lSDL2main -lSDL2
# to get rid of console: -mwindows

all: debug

debug: main.o logic.o render.o
	$(CC) main.o logic.o render.o -o main $(DEBUGARGS) 

release: main.o logic.o render.o
	$(CC) main.o logic.o render.o -omain $(RELEASEARGS)

%.o: %.c
	$(CC) -c $^ $(DEBUGARGS) 

clean:
	rm main.exe *.o