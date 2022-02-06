#include "map.h"

/* http://www.cse.yorku.ca/~oz/hash.html */
unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

Map mk_map()
{
    Map map;
    map.size = 0;
    map.data = calloc(MAP_SIZE, sizeof *map.data);
}

bool map_add(Map *map, char* id, SockData data)
{
    if (map->size > MAP_SIZE) return false;
    SockData* val = map_get(map, id);
    if (val != NULL) return false;
    *val = data;
    map->size++;
    return true;
}

SockData* map_get(Map *map, char* id) {
    return map->data + (hash(id) % MAP_SIZE);
}