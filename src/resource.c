#include "game.h"

SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *file) {
    SDL_Surface* surface = IMG_Load(file);
    if (surface == NULL) {
        printf("IMG_Load Error: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    }

    return texture;
}

SDL_Texture* loadCursor(SDL_Renderer *renderer, const char *file) {
    SDL_Surface* surface = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1);
    if (surface == NULL) {
        printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    }

    return texture;
}
