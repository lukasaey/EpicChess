#include <SDL2/SDL.h>
#include <stdio.h>

#include "game.h"
#include "render.h"
#include "logic.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg.h"
#include "nanosvgrast.h"

static PieceTexture pawn_texture;
static PieceTexture bishop_texture;
static PieceTexture knight_texture;
static PieceTexture rook_texture;
static PieceTexture queen_texture;
static PieceTexture king_texture;


SDL_Texture* svg_to_sdltexture(SDL_Renderer *renderer, char* filename, int w, int h)
{
    NSVGimage *svg_image = NULL;
	NSVGrasterizer *rast = NULL;

    svg_image = nsvgParseFromFile(filename, "px", 96);
    if (svg_image == NULL) {
		fprintf(stderr, "Could not open SVG image.\n");
		exit(1);
	}

	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		fprintf(stderr, "Could not init rasterizer.\n");
		exit(1);
	}

	unsigned char* img = malloc(w*h*4);
	if (img == NULL) {
		fprintf(stderr, "Could not alloc image buffer.\n");
		exit(1);
	}

    nsvgRasterize(rast, svg_image, 0,0, h / svg_image->height, img, w, h, w*4);

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
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    return tex;
}

void load_textures(SDL_Renderer *renderer, int w, int h)
{
    pawn_texture.white = svg_to_sdltexture(renderer, W_PAWN_FILEPATH, w, h);
    bishop_texture.white = svg_to_sdltexture(renderer, W_BISHOP_FILEPATH, w, h);
    knight_texture.white = svg_to_sdltexture(renderer, W_KNIGHT_FILEPATH, w, h);
    rook_texture.white = svg_to_sdltexture(renderer, W_ROOK_FILEPATH, w, h);
    queen_texture.white = svg_to_sdltexture(renderer, W_QUEEN_FILEPATH, w, h);
    king_texture.white = svg_to_sdltexture(renderer, W_KING_FILEPATH, w, h);
    
    pawn_texture.black = svg_to_sdltexture(renderer, B_PAWN_FILEPATH, w, h);
    bishop_texture.black = svg_to_sdltexture(renderer, B_BISHOP_FILEPATH, w, h);
    knight_texture.black = svg_to_sdltexture(renderer, B_KNIGHT_FILEPATH, w, h);
    rook_texture.black = svg_to_sdltexture(renderer, B_ROOK_FILEPATH, w, h);
    queen_texture.black = svg_to_sdltexture(renderer, B_QUEEN_FILEPATH, w, h);
    king_texture.black = svg_to_sdltexture(renderer, B_KING_FILEPATH, w, h);
}

void render_piece(uint8_t piece, int x, int y, SDL_Renderer *renderer)
{
    if (!piece) return;

    SDL_Rect rect = {
        .y = y*CELL_SIZE, 
        .x = x*CELL_SIZE,
        .h = SCREEN_SIZE/BOARD_N,
        .w = SCREEN_SIZE/BOARD_N,
    };

    bool is_white = piece & WHITE;
    piece &= 0b00111111; /* cut off color to make switch easier */


    PieceTexture tex_struct = {NULL};
    switch (piece) /* TODO: #3 lookup table? */
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

    SDL_Texture *tex = (is_white) ? tex_struct.white : tex_struct.black;
    SDL_RenderCopy(renderer, tex, NULL, &rect); 
}

void render_game(const game_t *game, SDL_Renderer *renderer) 
{
    uint32_t color = WHITE_SQUARE_COLOR; 
    size_t kingpos = INT_MAX;

    if (game->in_check) {
        kingpos = game->player == WHITE_PLAYER ? game->white_king_pos : game->black_king_pos;
    }

    for (size_t y = 0; y < BOARD_N; ++y) {
        for (size_t x = 0; x < BOARD_N; ++x) {
            SDL_Rect rect = {
                .x = x * CELL_SIZE,
                .y = y * CELL_SIZE,
                .w = CELL_SIZE,
                .h = CELL_SIZE,
            };

            uint32_t clr;
            size_t pos = y * BOARD_N + x;
            SDL_SetRenderDrawColor(renderer,
                (color & 0xff000000) >> 24,
                (color & 0xff0000) >> 16,
                (color & 0xff00) >> 8,
                 color & 0xff
            );
            SDL_RenderFillRect(renderer, &rect);

            if (game->selected == pos) 
                clr = SELECTED_SQUARE_COLOR;
            else if (pos == kingpos)
                clr = IN_CHECK_COLOR;
            else if (is_legal(game, game->selected, pos).legal) 
                clr = LEGAL_SQUARE_COLOR;
            else 
                clr = color;
            
            SDL_SetRenderDrawColor(renderer,
                (clr & 0xff000000) >> 24,
                (clr & 0xff0000) >> 16,
                (clr & 0xff00) >> 8,
                 clr & 0xff
            );
            SDL_RenderFillRect(renderer, &rect);
            
            if (game->board[pos] != 0) {
                render_piece(game->board[pos], x, y, renderer);
            }

            color = (color == WHITE_SQUARE_COLOR) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;
        }
        color = (color == WHITE_SQUARE_COLOR) ? BLACK_SQUARE_COLOR : WHITE_SQUARE_COLOR;
    }
}