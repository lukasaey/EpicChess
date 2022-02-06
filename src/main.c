#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "render.h"
#include "logic.h"

int SCREEN_SIZE = 800;

SDL_Window* window = NULL;
void cleanup()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error, could not init SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("EpicChess", 100, 100, 
        SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    window = window;

    /* in charge of all the cleanup */
    atexit(cleanup);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    load_textures(renderer, SCREEN_SIZE/BOARD_N, SCREEN_SIZE/BOARD_N);
    int cell_size = SCREEN_SIZE / BOARD_N;

    game_t game = DEFAULT_GAME_T;
    
    SDL_Event e;
    while (game.state != EXIT) 
    {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                game.state = EXIT;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (game.state == RUNNING) {
                    clicked_on_square(&game, e.button.x / cell_size,
                        e.button.y / cell_size);    
                }
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    SCREEN_SIZE = e.window.data2;
                    cell_size = SCREEN_SIZE / BOARD_N;
                    load_textures(renderer, SCREEN_SIZE/BOARD_N, SCREEN_SIZE/BOARD_N);
                }
                break;
            default: {}
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        render_game(&game, renderer);
        SDL_RenderPresent(renderer);
    }


    return EXIT_SUCCESS;
}