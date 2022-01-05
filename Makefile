DEBUGARGS=-g -Wall -Wextra -Wshadow -lmingw32 -lSDL2main -lSDL2 -lws2_32
RELEASEARGS=-D NDEBUG -O2 -mwindows -lmingw32 -lSDL2main -lSDL2 -lws2_32

SERVERDEBUGARGS=-g -Wall -Wextra -Wshadow -lws2_32
SERVERRELEASEARGS=-D NDEBUG -O2 -mwindows -lws2_32

_OBJS = main.o logic.o render.o net.o
OBJS = $(patsubst %,obj/%,$(_OBJS))

_SERVEROBJS = server.o logic.o net.o
SERVEROBJS = $(patsubst %,./obj/%,$(_SERVEROBJS))

all: debug server-debug

debug: $(OBJS)
	$(CC) $(OBJS) -o ./bin/main $(DEBUGARGS) 

release: $(OBJS)
	$(CC) $(OBJS) -o ./bin/main $(RELEASEARGS)

server-debug: $(SERVEROBJS)
	$(CC) $(SERVEROBJS) -o ./bin/server $(SERVERDEBUGARGS)

server-release: &(SERVEROBJS)
	$(CC) $(SERVEROBJS) -o ./bin/server $(SERVERRELEASEARGS)

obj/%.o: src/%.c
	$(CC) -c $^ -o $@ $ $(DEBUGARGS) 

clean:
	rm ./bin/server.exe ./bin/main.exe ./obj/*.o