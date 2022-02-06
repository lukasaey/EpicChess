#include "game.h"
#include "net.h"

#define MAP_SIZE 100

typedef struct {
    SOCKET *white;
    SOCKET *black;
} PlrPair;

typedef struct {
    game_t *game;
    PlrPair players;
} SockData;

typedef struct {
    SockData *data;
    size_t size;
} Map;

Map mk_map();
bool map_add(Map *map, char* id, SockData data);
SockData* map_get(Map *map, char* id);