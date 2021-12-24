CC=gcc
DEBUGARGS=-g -Wall -Wextra -Wshadow -lmingw32 -lSDL2main -lSDL2
RELEASEARGS=-O2 -mwindows -lmingw32 -lSDL2main -lSDL2

_OBJS = main.o logic.o render.o
OBJS = $(patsubst %,obj/%,$(_OBJS))

all: debug

debug: $(OBJS)
	$(CC) $(OBJS) -o ./bin/main $(DEBUGARGS) 

release: $(OBJS)
	$(CC) $(OBJS) -o ./bin/main $(RELEASEARGS)

obj/%.o: src/%.c
	$(CC) -c $^ -o $@ $ $(DEBUGARGS) 

clean:
	rm ./bin/main.exe ./obj/*