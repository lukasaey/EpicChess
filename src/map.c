#include "map.h"

/* http://www.cse.yorku.ca/~oz/hash.html */
inline
unsigned long
hashfunc(SOCKET a)
{  
    return ((5381 << 5) + 5381) + a;
}

inline
Map mk_map()
{
    Map map;
    map.size = 0;
    memset(map.data, 0, sizeof map.data);
}

bool map_add(Map *map, SOCKET sock, SockData data)
{
    if (map->size > MAP_SIZE) return false;
    assert(map->data[hashfunc(sock) % MAP_SIZE].valid);

    map->size++;
    data.valid = true;
    map->data[hashfunc(sock) % MAP_SIZE] = data;
    
    return true;
}