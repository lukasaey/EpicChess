#include <SDL2/SDL.h>
#include <stdio.h>

#include "game.h"
#include "render.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

PieceTexture pawn_texture;
PieceTexture bishop_texture;
PieceTexture knight_texture;
PieceTexture rook_texture;
PieceTexture queen_texture;
PieceTexture king_texture;


SDL_Texture* svgto_sdltexture(SDL_Renderer *renderer, char* filename)
{
    NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;
    unsigned char* img = NULL;
    int w, h;

    image = nsvgParseFromFile(filename, "px", SCREEN_WIDTH/2);
    if (image == NULL) {
		fprintf(stderr, "Could not open SVG image.\n");
		exit(1);
	}

    w = SCREEN_WIDTH;
	h = SCREEN_HEIGHT;

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		fprintf(stderr, "Could not init rasterizer.\n");
		exit(1);
	}

	img = malloc(w*h*4);
	if (img == NULL) {
		fprintf(stderr, "Could not alloc image buffer.\n");
		exit(1);
	}

    nsvgRasterize(rast, image, 0,0, 0.125, img, w, h, w*4);

    Uint32 rmask, gmask, bmask, amask;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif

    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(img, w, h, 32, 4*w,
                             rmask, gmask, bmask, amask);
    
    /* TODO: free surface and rast if possible */
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    return tex;
}

void preload_textures(SDL_Renderer *renderer)
{
    pawn_texture.white = svgto_sdltexture(renderer, W_PAWN_FILEPATH);
    bishop_texture.white = svgto_sdltexture(renderer, W_BISHOP_FILEPATH);
    knight_texture.white = svgto_sdltexture(renderer, W_KNIGHT_FILEPATH);
    rook_texture.white = svgto_sdltexture(renderer, W_ROOK_FILEPATH);
    queen_texture.white = svgto_sdltexture(renderer, W_QUEEN_FILEPATH);
    king_texture.white = svgto_sdltexture(renderer, W_KING_FILEPATH);
    
    pawn_texture.black = svgto_sdltexture(renderer, B_PAWN_FILEPATH);
    bishop_texture.black = svgto_sdltexture(renderer, B_BISHOP_FILEPATH);
    knight_texture.black = svgto_sdltexture(renderer, B_KNIGHT_FILEPATH);
    rook_texture.black = svgto_sdltexture(renderer, B_ROOK_FILEPATH);
    queen_texture.black = svgto_sdltexture(renderer, B_QUEEN_FILEPATH);
    king_texture.black = svgto_sdltexture(renderer, B_KING_FILEPATH);
}

void render_board(SDL_Renderer *renderer, const game_t *game)
{
    uint32_t color = WHITE_SQUARE_COLOR;

    for (Uint32 y = 0; y < BOARD_N; ++y) {
        for (Uint32 x = 0; x < BOARD_N; ++x) {
            SDL_Rect rect = {
                .x = x * (SCREEN_WIDTH/8),
                .y = y * (SCREEN_HEIGHT/8),
                .w = SCREEN_WIDTH/8,
                .h = SCREEN_HEIGHT/8,
            };

            if (y * BOARD_N + x == game->selected) {
                SDL_SetRenderDrawColor(renderer, 
                                (SELECTED_SQUARE_COLOR & 0xff000000) >> 24,
                                (SELECTED_SQUARE_COLOR & 0xff0000) >> 16,
                                (SELECTED_SQUARE_COLOR & 0xff00) >> 8,
                                SELECTED_SQUARE_COLOR & 0xff);
                SDL_RenderFillRect(renderer, &rect);
                color = (color == WHITE_SQUARE_COLOR) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;
                continue;
            }
            

            SDL_SetRenderDrawColor(renderer, 
                                (color & 0xff000000) >> 24,
                                (color & 0xff0000) >> 16,
                                (color & 0xff00) >> 8,
                                color & 0xff);

            SDL_RenderFillRect(renderer, &rect);
    
            color = (color == WHITE_SQUARE_COLOR) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;
        }
        color = (color == WHITE_SQUARE_COLOR) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;
    }
}

void render_piece(uint8_t piece, int x, int y, SDL_Renderer *renderer)
{
    if (!piece) return;

    SDL_Rect rect = {
        .y = y*CELL_SIZE, 
        .x = x*CELL_SIZE,
        .h = SCREEN_HEIGHT,
        .w = SCREEN_WIDTH,
    };

    uint8_t is_white = piece & WHITE;
    piece &= 0b00111111; /* cut off color to make switch easier */

    PieceTexture tex_struct = {NULL};
    switch (piece)
    {
        case PAWN:
            tex_struct = pawn_texture;
            break;
        case BISHOP:
            tex_struct = bishop_texture;
            break;
        case KNIGHT:
            tex_struct = knight_texture;
            break;
        case ROOK:
            tex_struct = rook_texture;
            break;
        case QUEEN:
            tex_struct = queen_texture;
            break;
        case KING:
            tex_struct = king_texture;
            break;
    }

    SDL_Texture *tex = (!!is_white) ? tex_struct.white : tex_struct.black;
    SDL_RenderCopy(renderer, tex, NULL, &rect); 
}

void render_game(const game_t *game, SDL_Renderer *renderer) 
{
    render_board(renderer, game);

    for (int y = 0; y < BOARD_N; ++y) {
        for (int x = 0; x < BOARD_N; ++x) {
            render_piece(game->board[y * BOARD_N + x], x, y, renderer);
        }
    }
}