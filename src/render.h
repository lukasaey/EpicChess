#ifndef RENDER_H
#define RENDER_H

typedef struct {
    SDL_Texture *white;
    SDL_Texture *black;
} PieceTexture;

void render_game(const game_t *game, SDL_Renderer *renderer);
void load_textures(SDL_Renderer *renderer, int w, int h);

#endif // RENDER_H
