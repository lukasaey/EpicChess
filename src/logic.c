#include "game.h"
#include "logic.h"

#include <stdbool.h>
#include <math.h>
#include <stdio.h>


bool is_line_empty
(const game_t *game, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    assert(x1 <= BOARD_N && x2 <= BOARD_N && y1 <= BOARD_N && y2 <= BOARD_N);

    int32_t x = x1;
    int32_t y = y1;

    int32_t *dynamic = (x1 == x2) ? &y : &x;
    int32_t limit = (x1 == x2) ? y2 : x2;
    int32_t step = *dynamic < limit ? 1 : -1;

    while (*dynamic != limit)
    {
        int32_t pos = y * BOARD_N + x;
        if (game->board[pos] != NO_PIECE &&
                pos != y2 * BOARD_N + x2 &&
                pos != y1 * BOARD_N + x1) 
            return false;
        *dynamic += step;
    }

    return true;
}

bool is_diag_empty /* TODO: improvements, like in is_line_empty */
(const game_t *game, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    assert(x1 <= BOARD_N && x2 <= BOARD_N && y1 <= BOARD_N && y2 <= BOARD_N);

    uint32_t min_x = min(x1, x2);
    uint32_t max_x = max(x1, x2); 
    uint32_t y = (min_x == x1) ? y1 : y2;
    uint32_t noty = (min_x != x1) ? y1 : y2;

    /* assuming that x1 - x2 == y1 - y2 */
    while (min_x <= max_x)
    {
        size_t pos = y * BOARD_N + min_x;
        uint8_t piece = game->board[pos];
        if (piece != NO_PIECE && pos != y1 * BOARD_N + x1 && pos != y2 * BOARD_N + x2)
            return false;
        min_x++; 
        if (y > noty) y--;
        else y++;
    }

    return true;
}

LegalInfo is_legal(const game_t *game, size_t origin, size_t dest)
{
    LegalInfo legal = {
        .legal = false,
        .enpassant = false,
        .firstmove = false,
        .castle = false,
        .white_castle = KING_CASTLE | QUEEN_CASTLE,
        .black_castle = KING_CASTLE | QUEEN_CASTLE,
    };

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

    if (piece != KING && !!(game->board[dest] & WHITE) == is_white 
            && game->board[dest] != 0) {
        return legal;
    }

    switch (piece) 
    {
    case PAWN: {
        bool one_ahead = x1 == x2 && y1 + 1 == y2 && game->board[dest] == NO_PIECE;  
        bool two_ahead = y1 == 1 && x1 == x2 && y1 + 2 == y2 
            && game->board[dest] == NO_PIECE 
            && game->board[dest+ (is_white ? BOARD_N : -BOARD_N)] == NO_PIECE;
        bool take = abs(x1 - x2) == 1 && y1 + 1 == y2 && game->board[dest] != NO_PIECE;
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
        /* Ys are calculated cuz they were normalized earlier on */
        bool diagonal = is_diag_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N);
        legal.legal = diagonal && abs(x1 - x2) == abs(y1 - y2);
        break;
    }
    case ROOK: {
        legal.legal = (x1 == x2 || y1 == y2) && is_line_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N);

        if (y1 == 0) {
            uint8_t *castle = is_white ? &legal.white_castle : &legal.black_castle;
            /* setting king or queen castle bit to 0 */
            if (x1 == 7) *castle &= ~KING_CASTLE; 
            else if (x1 == 0) *castle &= ~QUEEN_CASTLE; 
        } 

        break;
    }
    case QUEEN: {
        legal.legal = 
             (is_line_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N) 
                && (x1 == x2 || y1 == y2)) 
            || (is_diag_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N) 
                && abs(x1 - x2) == abs(y1 - y2));
        break;
    }
    case KING: {
        /* castling */
        //printf("w: %d, b: %d\n", game->white_castle, game->black_castle);
        if (y2 == 0) {
            uint8_t *castle = is_white ? &game->white_castle : &game->black_castle;
            if (x2 == 0) {
                bool clear = is_line_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N);
                bool can = *castle & QUEEN_CASTLE;
                legal.legal = clear && can;
                legal.castle = true;
                break;
            } 
            else if (x2 == 7) {
                bool clear = is_line_empty(game, x1, origin / BOARD_N, x2, dest / BOARD_N);
                bool can = *castle & KING_CASTLE;
                legal.legal = clear && can;
                legal.castle = true;
                break;
            }
        }

        if (!!(game->board[dest] & WHITE) == is_white && game->board[dest] != 0) {
            legal.legal = false;
            break;
        }
 
        legal.legal = abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1;

        if (legal.legal) {
            if (is_white) legal.white_castle = NO_CASTLING;
            else legal.black_castle = NO_CASTLING;
        } 
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
    game->black_castle &= legal.black_castle;
    game->white_castle &= legal.white_castle;
   
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

    if (legal.castle) {
        bool kingside = x == 7;
        if (kingside) {
            game->board[pos-2] = game->board[pos];
            game->board[pos] = NO_PIECE;
            game->board[pos-1] = game->board[game->selected];
            game->board[game->selected] = NO_PIECE;
            game->selected = NONE_SELECTED;
            return;
        } else {
            game->board[pos+2] = game->board[pos];
            game->board[pos] = NO_PIECE;
            game->board[pos+1] = game->board[game->selected];
            game->board[game->selected] = NO_PIECE;
            game->selected = NONE_SELECTED;
        }
    }

    game->board[pos] = game->board[game->selected];
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;
}