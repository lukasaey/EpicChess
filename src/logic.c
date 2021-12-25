#include "game.h"
#include "logic.h"

#include <stdbool.h>
#include <math.h>
#include <stdio.h>

LegalInfo is_legal(const game_t *game, size_t origin, size_t dest)
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

    LegalInfo legal = {
        .legal = false,
        .enpassant = false,
        .firstmove = false,
    };

    if (!!(game->board[dest] & WHITE) == is_white && game->board[dest] != 0) {
        return legal;
    }

    switch (piece) 
    {
    case PAWN: {
        bool one_ahead = x1 == x2 && y1 + 1 == y2 && game->board[dest] == 0;  
        bool two_ahead = y1 == 1 && x1 == x2 && y1 + 2 == y2 && game->board[dest] == 0;
        bool take = abs(x1 - x2) == 1 && y1 + 1 == y2 && game->board[dest] != 0;
        bool en_passant = game->en_passantable == dest + (is_white ? BOARD_N : -BOARD_N)
                          && abs(x1 - x2) == 1 && y1 + 1 == y2;
        legal.firstmove = two_ahead;
        legal.enpassant = en_passant;
        legal.legal = one_ahead || two_ahead || take || en_passant;
        break;
    }
    case KNIGHT: {
        bool vertL = abs(x1 - x2) == 1 && abs(y1 - y2) == 2;
        bool horzL = abs(x1 - x2) == 2 && abs(y1 - y2) == 1;
        legal.legal = vertL || horzL;
        break;
    }
    case BISHOP: {
        bool diagonal = false;
        legal.legal = diagonal;
        break;
    }
    case ROOK: {
        bool horizontal = false;
        bool vertical = false;
        legal.legal = horizontal || vertical;
        break;
    }
    case QUEEN: {
        bool horizontal = false;
        bool vertical = false;
        bool diagonal = false;
        legal.legal = horizontal || vertical || diagonal;
        break;
    }
    case KING: {
        bool one = false;
        legal.legal = one;
        break;
    }
    default: {}
    }

    return legal;
}

void clicked_on_square(game_t *game, int x, int y)
{
    unsigned int pos = y * BOARD_N + x;
    
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
    
    LegalInfo legal = is_legal(game, game->selected, pos);

    if (!legal.legal) {
        game->selected = NONE_SELECTED;
        return;
    }

    if (legal.firstmove) game->en_passantable = pos;

    if (legal.enpassant) {
        game->board[pos] = game->board[game->selected];
        game->board[game->en_passantable] = NO_PIECE;
        game->board[game->selected] = NO_PIECE;
        game->selected = NONE_SELECTED;
        game->en_passantable = NONE_SELECTED;
        return;
    }

    game->board[pos] = game->board[game->selected];
    /* TODO: #1 make this a bit smarter */
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;
}