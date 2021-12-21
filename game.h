#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <assert.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define BOARD_N 8
#define CELL_SIZE (SCREEN_WIDTH/BOARD_N)

static_assert(SCREEN_HEIGHT / SCREEN_WIDTH == 1, "Screen ratio must be 1:1");

#define W_PAWN_FILEPATH "./svg/wP.svg"
#define W_BISHOP_FILEPATH "./svg/wB.svg"
#define W_KNIGHT_FILEPATH "./svg/wN.svg"
#define W_ROOK_FILEPATH "./svg/wR.svg"
#define W_QUEEN_FILEPATH "./svg/wQ.svg"
#define W_KING_FILEPATH "./svg/wK.svg"

#define B_PAWN_FILEPATH "./svg/bP.svg"
#define B_BISHOP_FILEPATH "./svg/bB.svg"
#define B_KNIGHT_FILEPATH "./svg/bN.svg"
#define B_ROOK_FILEPATH "./svg/bR.svg"
#define B_QUEEN_FILEPATH "./svg/bQ.svg"
#define B_KING_FILEPATH "./svg/bK.svg"

#define NO_PIECE 0
#define PAWN   0b00000001
#define BISHOP 0b00000010
#define KNIGHT 0b00000100
#define ROOK   0b00001000
#define QUEEN  0b00010000
#define KING   0b00100000
#define BLACK  0b01000000
#define WHITE  0b10000000

#define NONE_SELECTED -1

/* RGBA */
#define WHITE_SQUARE_COLOR 0xefe4d4ff
#define BLACK_SQUARE_COLOR 0x535353ff
#define SELECTED_SQUARE_COLOR 0xd61515ff

typedef enum {
    WHITE_PLAYER,
    BLACK_PLAYER,
} player_t;

typedef struct {
    uint8_t board[BOARD_N * BOARD_N];
    player_t player;
    int selected;
} game_t;

#endif /* GAME_H */