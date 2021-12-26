#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

typedef struct {
    bool legal;
    bool firstmove;
    bool enpassant;
} LegalInfo;

void clicked_on_square(game_t *game, int x, int y);
LegalInfo is_legal(const game_t *game, size_t origin, size_t dest);


#endif // LOGIC_H
