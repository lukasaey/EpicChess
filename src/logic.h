#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

typedef struct {
    bool legal;
    bool firstmove;
    bool enpassant;
    bool castle;
    uint8_t white_castle;
    uint8_t black_castle;
} LegalInfo;

int clicked_on_square(game_t *game, int x, int y);
LegalInfo is_legal(const game_t *game, size_t origin, size_t dest);
bool in_check(const game_t *game, LegalInfo legal, size_t dest);


#endif // LOGIC_H
