#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "game.h"
#include "render.h"
#include "logic.h"
#include "net.h"

EGameType type = ONLINE_GAME;

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
    game_t game = DEFAULT_GAME_T;

    SOCKET sock;
    if (type == ONLINE_GAME) {
        sock = init_socket();
        if (sock == INVALID_SOCKET) 
            game.state = EXIT;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDOPERATION_ADD);

    const int cell_size = SCREEN_SIZE / BOARD_N;
    
    SDL_Event e;
    while (game.state != EXIT) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                game.state = EXIT;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (game.state == RUNNING) {
                    if (type == OFFLINE_GAME) {
                        clicked_on_square(&game, e.button.x / cell_size,
                                e.button.y / cell_size);    
                    } else {
                        int code = send_input(&game, sock, e.button.x / cell_size,
                                e.button.y / cell_size);
                        if (code) game.state = EXIT;
                    }
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

    if (type == ONLINE_GAME) {
        Header head = {
            .type = EXIT_HEADER,
        };
        /* sending just exit header, no error checking */
        send(sock, (char *)&head, sizeof head, 0);
        closesocket(sock);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}