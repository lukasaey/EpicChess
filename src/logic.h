#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>

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
LegalInfo _is_legal(const game_t *game, size_t origin, size_t dest);
bool is_check(const game_t *game, LegalInfo legal, size_t dest);
bool in_check(const game_t *game);

#endif // LOGIC_H
