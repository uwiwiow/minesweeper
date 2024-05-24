#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int WIDTH;
    int HEIGHT;
    int TILE;
} Status;

typedef struct {
    int x;
    int y;
} Coordinates;

typedef struct {
    int x;
    int y;
    int amount;
} Numbers;

// TODO hacer que no de dos coordenadas iguales
// TODO hacer que tengan poquito de probabilidad de que salgan juntas
void generateBombs(Coordinates *bombs, int count, Status status) {
    for (int i = 0; i < count; i++){
        bombs[i].x = rand() % (status.WIDTH / status.TILE);
        bombs[i].y = rand() % (status.HEIGHT / status.TILE);
    }
}

void generateNumbers(Numbers *numbers, Coordinates *bombs, int bombCount, Status status, int *numberCount) {
    int widthInTiles = status.WIDTH / status.TILE;
    int heightInTiles = status.HEIGHT / status.TILE;

    int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            {0, -1},          {0, 1},
            {1, -1}, {1, 0}, {1, 1}
    };

    *numberCount = 0;

    for (int y = 0; y < heightInTiles; y++) {
        for (int x = 0; x < widthInTiles; x++) {
            int count = 0;

            for (int d = 0; d < 8; d++) {
                int newX = x + directions[d][0];
                int newY = y + directions[d][1];

                if (newX >= 0 && newX < widthInTiles && newY >= 0 && newY < heightInTiles) {
                    for (int i = 0; i < bombCount; i++) {
                        if (bombs[i].x == newX && bombs[i].y == newY) {
                            count++;
                        }
                    }
                }
            }

            if (count > 0) {
                numbers[*numberCount].x = x;
                numbers[*numberCount].y = y;
                numbers[*numberCount].amount = count;
                *numberCount += 1;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    time_t t;
    srand((unsigned)time(&t));

    Status status = {
        .TILE = 40,
        .WIDTH = 32*40,
        .HEIGHT = 16*40
    };

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
#endif

    int fps = 60;
    const int DELAY_TIME = 1000 / fps;

    Uint64 frameStart;
    Uint64 frameTime;

    // Create window
    SDL_Window *window = SDL_CreateWindow("Busca minas xdxd",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          status.WIDTH, status.HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {

            int bombCount = 99;
            Coordinates bombs[bombCount];
            generateBombs(bombs, bombCount, status);

            int numberCount = bombCount*8;
            Numbers numbers[numberCount];
            generateNumbers(numbers, bombs, bombCount, status,  &numberCount);

            // TODO quit this
            for (int i = 0; i < bombCount; i++) {
                printf("%d %d\n", bombs[i].x, bombs[i].y);
            }
            printf("numbers\n\n");
            for (int i = 0; i < numberCount; i++) {
                printf("%d %d %d\n", numbers[i].x, numbers[i].y, numbers[i].amount);
            }

            // Declare rect of square
            SDL_Rect pointerSquareRect;

            // Square dimensions
            pointerSquareRect.w = status.TILE;
            pointerSquareRect.h = status.TILE;

            // Square position
            pointerSquareRect.x = 0;
            pointerSquareRect.y = 0;

            // Event loop exit flag
            bool quit = false;

            while (!quit) {
                frameStart = SDL_GetTicks64();
                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    // User requests quit
                    if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN) {

                        if (e.key.keysym.sym == SDLK_a) {
                            pointerSquareRect.x -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_d) {
                            pointerSquareRect.x += status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_w) {
                            pointerSquareRect.y -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_s) {
                            pointerSquareRect.y += status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_k) {
                            generateBombs(bombs, bombCount, status);
                            generateNumbers(numbers, bombs, bombCount, status,  &numberCount);
                        }

                    }
                }

                if (pointerSquareRect.x == status.WIDTH) {
                    pointerSquareRect.x = 0;
                }

                if (pointerSquareRect.x == -status.TILE) {
                    pointerSquareRect.x = status.WIDTH - status.TILE;
                }

                if (pointerSquareRect.y == status.WIDTH) {
                    pointerSquareRect.y = 0;
                }

                if (pointerSquareRect.y == -status.TILE) {
                    pointerSquareRect.y = status.HEIGHT - status.TILE;
                }


                // 0x declares hex number   FF= 255
                // Initialize renderer color white for the background
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // Clear screen
                SDL_RenderClear(renderer);

                for (int i = 0; i < numberCount; i++) {
                    SDL_Rect rect = {numbers[i].x * status.TILE,
                                     numbers[i].y  * status.TILE,
                                     status.TILE,
                                     status.TILE};

                    SDL_SetRenderDrawColor(renderer, 0x00, 250-(30*numbers[i].amount), 0x44, 0xFF);
                    SDL_RenderFillRect(renderer, &rect);
                }

                for (int i = 0; i < bombCount; i++) {
                    SDL_Rect rect = {bombs[i].x * status.TILE,
                                     bombs[i].y  * status.TILE,
                                     status.TILE,
                                     status.TILE};

                    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
                    SDL_RenderFillRect(renderer, &rect);
                }

                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x55);
                SDL_RenderFillRect(renderer, &pointerSquareRect);


                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

                // Update screen
                SDL_RenderPresent(renderer);

                frameTime = SDL_GetTicks64() - frameStart;

                // Esperar el tiempo restante hasta alcanzar el frame rate deseado
                if (frameTime < DELAY_TIME) {
                    SDL_Delay(DELAY_TIME - frameTime);
                }

            }

            // Destroy renderer
            SDL_DestroyRenderer(renderer);

        }

        // Destroy window
        SDL_DestroyWindow(window);

    }

    // Quit SDL
    SDL_Quit();


    return 0;

}


