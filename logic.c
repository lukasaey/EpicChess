#include "game.h"
#include "logic.h"

bool is_legal(const game_t *game, int origin, int dest)
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
        bool two_ahead = y1 == 1 && x1 == x2 && y1 + 2 == y2;
        bool take = abs(x1 - x2) == 1 && y1 + 1 == y2 && game->board[dest] != 0;
        bool en_passant = false; /* TODO: #2 en passant */
        return one_ahead || two_ahead || take || en_passant;
    }
    case KNIGHT: {
        bool L = false;
        return L;
    }
    case BISHOP: {
        bool diagnal = false;
        return diagnal;
    }
    case ROOK: {
        bool horizontal = false;
        bool vertical = false;
        return horizontal || vertical;
    }
    case QUEEN: {
        bool horizontal = false;
        bool vertical = false;
        bool diag = false;
        return horizontal || vertical || diag;
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
    int pos = y * BOARD_N + x;
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

    if (!is_legal(game, game->selected, pos)) {
        game->selected = NONE_SELECTED;
        return;
    }

    game->board[pos] = game->board[game->selected];
    /* TODO: #1 make this a bit smarter */
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;
}