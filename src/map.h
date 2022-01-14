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
    bool valid;
} SockData;

typedef struct {
    SockData data[MAP_SIZE];
    size_t size;
} Map;

inline Map mk_map();
bool map_add(Map *map, SOCKET sock, SockData data);
SockData map_get(Map *map, SOCKET sock);