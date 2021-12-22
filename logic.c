#include "game.h"
#include "logic.h"

#include <stdbool.h>
#include <math.h>
#include <stdio.h>

bool is_legal(const game_t *game, size_t origin, size_t dest, bool *enpassant, bool *firstmove)
{
    bool is_white = game->board[origin] & WHITE;
    uint8_t piece = game->board[origin] & 0b00111111; /* without color */ 

    int x1, y1, x2, y2;
    x1 = origin % BOARD_N;
    y1 = origin / BOARD_N;
    x2 = dest % BOARD_N;
    y2 = dest / BOARD_N;

    if (is_white) {
        y1 = 7 - y1;
        y2 = 7 - y2;
    }

    switch (piece) 
    {
    case PAWN: {
        bool one_ahead = x1 == x2 && y1 + 1 == y2 && game->board[dest] == 0;  
        bool two_ahead = y1 == 1 && x1 == x2 && y1 + 2 == y2 && game->board[dest] == 0;
        bool take = abs(x1 - x2) == 1 && y1 + 1 == y2 && game->board[dest] != 0;
        bool en_passant = game->en_passantable == dest + (is_white ? BOARD_N : -BOARD_N);
        *firstmove = two_ahead;
        *enpassant = en_passant;
        return one_ahead || two_ahead || take || en_passant;
    }
    case KNIGHT: {
        bool L = false;
        return L;
    }
    case BISHOP: {
        bool diagonal = false;
        return diagonal;
    }
    case ROOK: {
        bool horizontal = false;
        bool vertical = false;
        return horizontal || vertical;
    }
    case QUEEN: {
        bool horizontal = false;
        bool vertical = false;
        bool diagonal = false;
        return horizontal || vertical || diagonal;
    }
    case KING: {
        bool one = false;
        return one;
    }
    default: {}
    }

    return true;
}

void clicked_on_square(game_t *game, int x, int y)
{
    unsigned int pos = y * BOARD_N + x;
    bool is_white = game->board[pos] & WHITE;

    if (game->selected == NONE_SELECTED) {
        if (game->board[pos] != 0) {
            game->selected = pos; 
        }
        return;
    }

    if (pos == game->selected) {
        game->selected = NONE_SELECTED;
        return;
    }

    if ((game->board[game->selected] & WHITE) == is_white && 
        game->board[pos] != 0) {
        game->selected = NONE_SELECTED;
        return;
    }
    
    bool enpassant, firstmove;

    if (!is_legal(game, game->selected, pos, &enpassant, &firstmove)) {
        game->selected = NONE_SELECTED;
        return;
    }

    if (firstmove) game->en_passantable = pos;

    if (enpassant) {
        game->board[pos] = game->board[game->selected];
        game->board[game->en_passantable] = NO_PIECE;
        game->board[game->selected] = NO_PIECE;
        game->selected = NONE_SELECTED;
        game->en_passantable = NONE_SELECTED;
        return;
    }

    printf("x: %lld, y: %lld", game->en_passantable % 8, game->en_passantable % 8);

    game->board[pos] = game->board[game->selected];
    /* TODO: #1 make this a bit smarter */
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;
}