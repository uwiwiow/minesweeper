#include "game.h"

int initializeSDL(Status *status, TILE **board) {
    SDL_SetMainReady();

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized!\nSDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }

    return 1;
}

void cleanupSDL(Status status, TILE **board) {
    TTF_Quit();
    SDL_Quit();
    freeMem(status, board);
}
