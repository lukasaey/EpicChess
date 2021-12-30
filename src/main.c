#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "render.h"
#include "logic.h"

const uint8_t DEFAULT_BOARD[64] =
{
    BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN, BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK,
    BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN, BLACK | PAWN,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,  
    WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN, WHITE | PAWN,
    WHITE | ROOK, WHITE | KNIGHT, WHITE | BISHOP, WHITE | QUEEN, WHITE | KING, WHITE | BISHOP, WHITE | KNIGHT, WHITE | ROOK,
};

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error, could not init SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("EpicChess", 100, 100,
                                 SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                         SDL_RENDERER_ACCELERATED |
                                         SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }


    preload_textures(renderer);
    game_t game = {
        .board = {0}, 
        .player = WHITE_PLAYER,
        .selected = NONE_SELECTED,
        .en_passantable = NONE_SELECTED,
        .black_castle = KING_CASTLE | QUEEN_CASTLE,
        .white_castle = KING_CASTLE | QUEEN_CASTLE,
        .black_king_pos = 4,
        .white_king_pos = 60,
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDOPERATION_ADD);

    memcpy(game.board, DEFAULT_BOARD, 64);

    const int cell_size = SCREEN_SIZE / BOARD_N;
    
    SDL_Event e;
    int quit = 0;
    clock_t t;
    while (!quit) {
        t = clock();
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                clicked_on_square(&game, e.button.x / cell_size,
                                        e.button.y / cell_size);
                break;
            default: {}
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        render_game(&game, renderer);
        SDL_RenderPresent(renderer);
        unsigned int elapsed = (clock() - t) / CLOCKS_PER_SEC;
        if (elapsed > DT) sleep(elapsed);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}