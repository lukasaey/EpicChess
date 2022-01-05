#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define SCREEN_SIZE 800
#define BOARD_N 8
#define CELL_SIZE (SCREEN_SIZE/BOARD_N)

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

#define NO_CASTLING  0
#define KING_CASTLE  0b01
#define QUEEN_CASTLE 0b10

#define NONE_SELECTED ((BOARD_N*BOARD_N)+1)

/* RGBA */
#define WHITE_SQUARE_COLOR 0xefe4d4ff
#define BLACK_SQUARE_COLOR 0x535353ff
#define SELECTED_SQUARE_COLOR 0xd61515ff
#define LEGAL_SQUARE_COLOR 0xed3232a1 
#define IN_CHECK_COLOR 0x930eabaf

#define DEFAULT_GAME_T \
{ \
    .board = { \
        BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN, BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK, \
        BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        0,0,0,0,0,0,0,0, \
        WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, \
        WHITE | ROOK, WHITE | KNIGHT, WHITE | BISHOP, WHITE | QUEEN, WHITE | KING, WHITE | BISHOP, WHITE | KNIGHT, WHITE | ROOK, \
    }, \
    .player = WHITE_PLAYER, \
    .state = RUNNING, \
    .selected = NONE_SELECTED, \
    .en_passantable = NONE_SELECTED, \
    .black_castle = KING_CASTLE | QUEEN_CASTLE, \
    .white_castle = KING_CASTLE | QUEEN_CASTLE, \
    .black_king_pos = 4, \
    .white_king_pos = 60, \
    .in_check = false, \
} \

typedef enum EGameType {
    OFFLINE_GAME,
    ONLINE_GAME,
} EGameType;

typedef enum {
    WHITE_PLAYER,
    BLACK_PLAYER,
} EPlayer;

typedef enum {
    EXIT,
    RUNNING,
    ENDED,
} EGameState;

typedef struct {
    uint8_t board[BOARD_N * BOARD_N];
    EPlayer player;
    EGameState state;
    uint8_t white_castle;
    uint8_t black_castle;
    unsigned int selected;
    unsigned int en_passantable; /* keeping track of an enpassantable pawn, if it exists */
    int white_king_pos; /* just to make check checking easier */
    int black_king_pos;
    bool in_check;
} game_t;

#endif /* GAME_H */