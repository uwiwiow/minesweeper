#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int WIDTH;
    int HEIGHT;
    int W_TILES;
    int H_TILES;
    int TILE;
    int BOMBS;
} Status;

typedef struct {
    int TYPE; // 0 none, 1 number, 2 mine
    int AMOUNT; // for number
    bool VISIBLE;
} Cell;

typedef struct {
    int X;
    int Y;
} Coords;

void initializeBoard(Cell **board, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            board[i][j].TYPE = 0;
            board[i][j].AMOUNT = 0;
            board[i][j].VISIBLE = false;

        }
    }
}

void freeMem(Status status, Cell **board) {
    for (int i = 0; i < status.W_TILES; i++) {
        free(board[i]);
    }
    free(board);
    printf("deallocated\n");
}

void generateBombs(Cell **board, Coords *bombs, int count, Status status) {
    int x, y;

    for (int i = 0; i < count; i++) {
        x = rand() % status.W_TILES;
        y = rand() % status.H_TILES;
        bombs[i].X = x;
        bombs[i].Y = y;
        board[x][y].TYPE = 2;
    }
}

void generateNumbers(Cell **board, Coords *bombs, Status status) {
    int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            {0, -1},          {0, 1},
            {1, -1}, {1, 0}, {1, 1}
    };

    for (int x = 0; x < status.W_TILES; x++) {
        for (int y = 0; y < status.H_TILES; y++) {
            if (board[x][y].TYPE != 2) {
                for (int d = 0; d < 8; d++) {
                    int newX = x + directions[d][0];
                    int newY = y + directions[d][1];

                    if (newX >= 0 && newX < status.W_TILES && newY >= 0 && newY < status.H_TILES) {
                        if (board[newX][newY].TYPE == 2) {
                            board[x][y].TYPE = 1;
                            board[x][y].AMOUNT += 1;
                        }
                    }
                }
            }
        }
    }
}


char* intToString(int x, int y) {
    char* result = malloc(20 * sizeof(char));
    sprintf(result, "(%d, %d)", x, y);
    return result;
}

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, int x, int y, SDL_Color color) {
    char* text = intToString(x, y);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
    free(text);
    if (surfaceMessage == NULL) {
        printf("TTF_RenderText_Solid Error: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Texture* textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if (textureMessage == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return NULL;
    }

    return textureMessage;
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
        .HEIGHT = 16*40,
        .W_TILES = 32,
        .H_TILES = 16,
        .BOMBS = 99
    };

    Cell **board = malloc(status.W_TILES * sizeof(Cell *));
    for (int i = 0; i < status.W_TILES; i++) {
        board[i] = malloc(status.H_TILES * sizeof(Cell));
    }

    initializeBoard(board, status.W_TILES, status.H_TILES);

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        freeMem(status, board);
        return 1;
    }

    if (TTF_Init() != 0) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        freeMem(status, board);
        return 1;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        SDL_Quit();
        freeMem(status, board);
        return 1;
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


            TTF_Font *font = TTF_OpenFont("/usr/share/fonts/noto/NotoSerif-Regular.ttf", 24);
            if (font == NULL) {
                printf("TTF_OpenFont Error: %s\n", TTF_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_Quit();
                SDL_Quit();
                freeMem(status, board);
                return 1;
            }

            SDL_Color textColor = {0, 0, 0, 255};
            SDL_Rect messageRect = {50, 50, 200, 100};
            SDL_Texture* message = renderText(renderer, font, 0, 0, textColor);
            if (message == NULL) {
                TTF_CloseFont(font);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                TTF_Quit();
                SDL_Quit();
                return 1;
            };
            bool printMessage = false;



            Coords bombs[status.BOMBS];
            generateBombs(board, bombs, status.BOMBS, status);
            generateNumbers(board, bombs, status);



            SDL_Surface* pointerSurface = SDL_LoadBMP_RW(SDL_RWFromFile("/home/uwiwiow/Documents/minesweeper/pointer.bmp", "rb"), 1);
            if (pointerSurface == NULL) {
                printf("SDL_LoadBMP Error: %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_Texture* pointerTexture = SDL_CreateTextureFromSurface(renderer, pointerSurface);
            SDL_FreeSurface(pointerSurface);
            if (pointerTexture == NULL) {
                printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_Rect pointerRect = {0, 0, status.TILE, status.TILE};

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
                            pointerRect.x -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_d) {
                            pointerRect.x += status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_w) {
                            pointerRect.y -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_s) {
                            pointerRect.y += status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_k) {
                            initializeBoard(board, status.W_TILES, status.H_TILES);
                            generateBombs(board, bombs, status.BOMBS, status);
                            generateNumbers(board, bombs, status);
                        }

                        if (e.key.keysym.sym == SDLK_p) {
                            printMessage = !printMessage;
                        }



                    }
                }

                if (pointerRect.x == status.WIDTH) {
                    pointerRect.x = 0;
                }

                if (pointerRect.x == -status.TILE) {
                    pointerRect.x = status.WIDTH - status.TILE;
                }

                if (pointerRect.y == status.HEIGHT) {
                    pointerRect.y = 0;
                }

                if (pointerRect.y == -status.TILE) {
                    pointerRect.y = status.HEIGHT - status.TILE;
                }


                // Initialize renderer color white for the background
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // Clear screen
                SDL_RenderClear(renderer);

                for (int x = 0; x < status.W_TILES; x++) {
                    for (int y = 0; y < status.H_TILES; y++) {
                        if (board[x][y].VISIBLE) {
                            if (board[x][y].TYPE == 1) {
                                SDL_Rect rect = {x * status.TILE,
                                                 y * status.TILE,
                                                 status.TILE,
                                                 status.TILE};
                                SDL_SetRenderDrawColor(renderer, 0x00, 250-(30*board[x][y].AMOUNT), 0x44, 0xFF);
                                SDL_RenderFillRect(renderer, &rect);
                            }
                            if (board[x][y].TYPE == 2) {
                                SDL_Rect rect = {x * status.TILE,
                                                 y * status.TILE,
                                                 status.TILE,
                                                 status.TILE};
                                SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
                                SDL_RenderFillRect(renderer, &rect);
                            }
                        }
                    }
                }

                SDL_RenderCopy(renderer, pointerTexture, NULL, &pointerRect);

                if (printMessage) {
                    message = renderText(renderer, font, (pointerRect.x / status.TILE), (pointerRect.y / status.TILE), textColor);
                    if (message == NULL) {
                        TTF_CloseFont(font);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        TTF_Quit();
                        SDL_Quit();
                        return 1;
                    }
                    messageRect.y = 50;
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, board[(pointerRect.x / status.TILE)][(pointerRect.y / status.TILE)].TYPE, board[(pointerRect.x / status.TILE)][(pointerRect.y / status.TILE)].AMOUNT, textColor);
                    if (message == NULL) {
                        TTF_CloseFont(font);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        TTF_Quit();
                        SDL_Quit();
                        return 1;
                    }
                    messageRect.y = -20;
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);
                }


                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

                // Update screen
                SDL_RenderPresent(renderer);

                frameTime = SDL_GetTicks64() - frameStart;

                if (frameTime < DELAY_TIME) {
                    SDL_Delay(DELAY_TIME - frameTime);
                }

            }
            SDL_DestroyTexture(message);
            SDL_DestroyTexture(pointerTexture);
            TTF_CloseFont(font);

            // Destroy renderer
            SDL_DestroyRenderer(renderer);

        }

        // Destroy window
        SDL_DestroyWindow(window);

    }

    // Quit SDL
    SDL_Quit();

    freeMem(status, board);


    return 0;

}


