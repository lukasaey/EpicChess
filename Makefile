LIBS=-lmingw32 -lSDL2main -lSDL2

DEBUGARGS=-g -Wall -Wextra -Wshadow $(LIBS)
RELEASEARGS=-D NDEBUG -O2 -mwindows $(LIBS)

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
	rm ./bin/main.exe ./obj/*.o