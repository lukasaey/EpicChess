#include "game.h"
#include "logic.h"

void clicked_on_square(game_t *game, int x, int y)
{
    int pos = y * BOARD_N + x;
    if (pos == game->selected) {
        game->selected = NONE_SELECTED;
        return;
    }

    if (game->selected == NONE_SELECTED) {
        if (game->board[pos] != 0) {
            game->selected = pos; 
        }
        return;
    }

    game->board[pos] = game->board[game->selected];
    game->board[game->selected] = NO_PIECE;
    game->selected = NONE_SELECTED;
}