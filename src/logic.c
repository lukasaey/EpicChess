#include "game.h"
#include "logic.h"
#include "net.h"

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

bool is_line_empty
(const game_t *game, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    assert(x1 <= BOARD_N && x2 <= BOARD_N && y1 <= BOARD_N && y2 <= BOARD_N);

    int x = x1, y = y1;
    int *dynamic = (x1 == x2) ? &y : &x;
    int limit = (x1 == x2) ? y2 : x2;
    int step = *dynamic < limit ? 1 : -1;

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

void move_piece(game_t *game, LegalInfo legal, int x, int y)
{
    int pos = y * BOARD_N + x;

    if (legal.enpassant) {
        game->board[pos] = game->board[game->selected];
        game->board[game->en_passantable] = NO_PIECE;
        game->board[game->selected] = NO_PIECE;
        game->selected = NONE_SELECTED;
        game->en_passantable = NONE_SELECTED;
        return;
    }

    uint8_t piece = game->board[game->selected];

    if (legal.castle) {
        int sign = (x == 7) ? -1 : 1;
        game->board[pos+(2*sign)] = game->board[pos];
        game->board[pos] = NO_PIECE;
        game->board[pos+sign] = game->board[game->selected];
        game->board[game->selected] = NO_PIECE;
        game->selected = NONE_SELECTED;

        int *king_pos = (piece & WHITE) ? &game->white_king_pos : &game->black_king_pos;
        *king_pos = pos+sign;

        return;
    }

    if (piece & KING) {
        int *king_pos = (piece & WHITE) ? &game->white_king_pos : &game->black_king_pos;
        *king_pos = pos;
    }

    if (legal.firstmove) game->en_passantable = pos;

    game->board[pos] = game->board[game->selected];
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;

    return;
}

bool* get_legal_moves(const game_t *game, int pos)
{
    static bool moves[BOARD_N*BOARD_N];

    for (int i = 0; i < BOARD_N*BOARD_N; ++i) 
    {
        /* using regular is_legal causes infinite recursion */
        if (_is_legal(game, pos, i).legal)
            moves[i] = true;
        else
            moves[i] = false;
    }

    return moves;
}

bool in_check(const game_t *game)
{
    uint8_t color = game->player == WHITE_PLAYER ? WHITE : BLACK;
    int king_pos = color == WHITE ? game->white_king_pos : game->black_king_pos;

    for (int i = 0; i < BOARD_N*BOARD_N; ++i)
    {
        bool *legalmoves = get_legal_moves(game, i);

        if (game->board[i] != color && legalmoves[king_pos]) {
            return true;
        }
    }    

    return false;
}

/* wrapper for in_check that checks a move instead of the current state */
bool is_check(const game_t *game, LegalInfo legal, size_t dest)
{   
    int x = dest % BOARD_N, y = dest / BOARD_N;
    /* using a copy of game */
    game_t mygame = *game;
    move_piece(&mygame, legal, x, y);

    return in_check(&mygame);
}

bool check_mate(const game_t *game)
{
    int kingpos = game->player == WHITE_PLAYER ? game->white_king_pos : game->black_king_pos;
    
    size_t counter = 0;

    for (int i = 0; i < BOARD_N*BOARD_N; ++i) 
    //for (int j = 0; j < BOARD_N*BOARD_N; ++j)
    {
        if (is_legal(game, kingpos, i).legal) {
            counter++;
        }
    }
    /* the square the piece is on counts as legal, so we gotta get rid of it */
    counter--;
    return counter == 0 && game->in_check;
}

LegalInfo _is_legal(const game_t *game, size_t origin, size_t dest)
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

    int x1 = origin % BOARD_N;
    int y1 = origin / BOARD_N;
    int x2 = dest % BOARD_N;
    int y2 = dest / BOARD_N;

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
        /* Ys are calculated cuz they were changed earlier on */
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
        if (y2 == 0 && !game->in_check) {
            const uint8_t *castle = is_white ? &game->white_castle : &game->black_castle;
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

        /* since castling involves clicking on a same color, it's handled here instead */
        if (!!(game->board[dest] & WHITE) == is_white && game->board[dest] != 0) {
            legal.legal = false;
            break;
        }
 
        legal.legal = abs(x1 - x2) <= 1 && abs(y1 - y2) <= 1;

        /* moving the king automatically disqualifies castling */
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

/* wrapper for both functions */
LegalInfo is_legal(const game_t *game, size_t origin, size_t dest)
{
    LegalInfo legal = _is_legal(game, origin, dest);
    legal.legal = legal.legal && !is_check(game, legal, dest);
    return legal;
}

int send_input(game_t *game, SOCKET sock, int x, int y)
{
    if (game->selected == NONE_SELECTED) {
        game->selected = y * BOARD_N + x;
        return 0;
    }

    Header head = {
        .type = GAME_HEADER,
    };

    GameData data = {
        .x1 = game->selected % BOARD_N,
        .y1 = game->selected / BOARD_N,
        .x2 = x,
        .y2 = y,
    };

    if (send_move(sock, game, &head, &data)) return 1;
    game->selected = NONE_SELECTED;

    return 0;
}

int clicked_on_square(game_t *game, int x, int y)
{
    unsigned int pos = y * BOARD_N + x;
    
    if (game->selected == NONE_SELECTED) {
        if (game->board[pos] != 0 && ((game->board[pos] & WHITE && game->player == WHITE_PLAYER) || ( game->board[pos] & BLACK && game->player == BLACK_PLAYER))) {
            game->selected = pos; 
        }
        return 1;
    }

    if (pos == game->selected) {
        game->selected = NONE_SELECTED;
        return 1;
    }

    LegalInfo legal = is_legal(game, game->selected, pos);
    
    if (!legal.legal) {
        game->selected = NONE_SELECTED;
        return 1;
    }

    game->black_castle &= legal.black_castle;
    game->white_castle &= legal.white_castle;

    move_piece(game, legal, x, y);

    if (check_mate(game)) {
        printf("%s won by checkmate", game->player == WHITE_PLAYER ? "black" : "white");
        game->state = ENDED;
    }
    game->player = game->player == WHITE_PLAYER ? BLACK_PLAYER : WHITE_PLAYER;
    game->in_check = in_check(game);
    return 0;
}