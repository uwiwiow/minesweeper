#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    START,
    PLAYING,
    WIN,
    LOSE
} State;

typedef struct {
    int WIDTH;
    int HEIGHT;
    int W_TILES;
    int H_TILES;
    int TILE;
    int BOMBS;
    int VISIBLE_TILES;
    State STATE;
} Status;

typedef enum {
    CELL_CLEARED,
    CELL_FLAGGED,
    CELL_QUESTIONED
} CellMark;

typedef enum {
    BLANK,
    NUMBER,
    MINE,
    MINE_EXPLOSION
} CellType;

typedef struct {
    CellType TYPE;
    int AMOUNT;
    CellMark MARK;
    bool VISIBLE;
} TILE;

void initializeBoard(TILE **board, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            board[i][j].TYPE = BLANK;
            board[i][j].AMOUNT = 0;
            board[i][j].VISIBLE = false;
            board[i][j].MARK = CELL_CLEARED;
        }
    }
}

void freeMem(Status status, TILE **board) {
    for (int i = 0; i < status.W_TILES; i++) {
        free(board[i]);
    }
    free(board);
}

void generateBombs(TILE **board, int count, Status status) {
    int x, y;

    for (int i = 0; i < count; i++) {
        x = rand() % status.W_TILES;
        y = rand() % status.H_TILES;
        board[x][y].TYPE = MINE;
    }
}

void generateNumbers(TILE **board, Status *status) {
    int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
    };
    status->BOMBS = 0;
    for (int x = 0; x < status->W_TILES; x++) {
        for (int y = 0; y < status->H_TILES; y++) {
            if (board[x][y].TYPE != MINE) {
                for (int d = 0; d < 8; d++) {
                    int newX = x + directions[d][0];
                    int newY = y + directions[d][1];

                    if (newX >= 0 && newX < status->W_TILES && newY >= 0 && newY < status->H_TILES) {
                        if (board[newX][newY].TYPE == MINE) {
                            board[x][y].TYPE = NUMBER;
                            board[x][y].AMOUNT += 1;
                        }
                    }
                }
            } else {
                status->BOMBS += 1;
            }
        }
    }
}

void revealEmptyCells(TILE **board, int x, int y, Status *status) {
    int directions[8][2] = {
            {-1, -1}, {-1, 0}, {-1, 1},
            { 0, -1},          { 0, 1},
            { 1, -1}, { 1, 0}, { 1, 1}
    };

    if (x < 0 || x >= status->W_TILES || y < 0 || y >= status->H_TILES || board[x][y].VISIBLE) {
        return;
    }

    if (board[x][y].MARK == CELL_FLAGGED) {
        return;
    }

    board[x][y].VISIBLE = true;
    status->VISIBLE_TILES += 1;

    if (board[x][y].TYPE == MINE) {
        board[x][y].TYPE = MINE_EXPLOSION;
        status->STATE = LOSE;
        return;
    }

    if ((status->VISIBLE_TILES + status->BOMBS) == (status->W_TILES * status->H_TILES)) {
        status->STATE = WIN;
    }

    if (board[x][y].TYPE == NUMBER) {
        return;
    }


    for (int d = 0; d < 8; d++) {
        int newX = x + directions[d][0];
        int newY = y + directions[d][1];
        revealEmptyCells(board, newX, newY, status);
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
        .BOMBS = 99,
        .STATE = START,
        .VISIBLE_TILES = 0
    };

    Status defaultStatus = status;

    TILE **board = malloc(status.W_TILES * sizeof(TILE *));
    for (int i = 0; i < status.W_TILES; i++) {
        board[i] = malloc(status.H_TILES * sizeof(TILE));
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
    SDL_Window *window = SDL_CreateWindow("Minesweeper",
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

            // LOAD FONT
            TTF_Font *font = TTF_OpenFont("assets/NotoSerif-Regular.ttf", 24);
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
            }
            bool printMessage = false;


            // GENERATE BOMBS And NUMBERS
            generateBombs(board, status.BOMBS, status);
            generateNumbers(board, &status);


            // LOAD CURSOR SPRITE
            SDL_Surface* pointerSurface = SDL_LoadBMP_RW(SDL_RWFromFile("assets/pointer.bmp", "rb"), 1);
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
            SDL_Rect cursorRect = {0, 0, status.TILE, status.TILE};



            // LOAD SPRITE ATLAS
            SDL_Surface* surface = IMG_Load("assets/atlas.png");
            if (surface == NULL) {
                printf("IMG_Load Error: %s\n", IMG_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            if (texture == NULL) {
                printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 1;
            }

            SDL_Rect spriteRectangles[16];

            int spriteIndex = 0;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    spriteRectangles[spriteIndex].x = col * 16;
                    spriteRectangles[spriteIndex].y = row * 16;
                    spriteRectangles[spriteIndex].w = 16;
                    spriteRectangles[spriteIndex++].h = 16;
                }
            }

            bool quit = false;
            bool setVisibleTiles = false;

            int rectX, rectY;

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
                            cursorRect.x -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_d) {
                            cursorRect.x += status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_w) {
                            cursorRect.y -= status.TILE;
                        }

                        if (e.key.keysym.sym == SDLK_s) {
                            cursorRect.y += status.TILE;
                        }

                        rectX = cursorRect.x/status.TILE;
                        rectY = cursorRect.y/status.TILE;

                        if (e.key.keysym.sym == SDLK_i) {
                            if (status.STATE = START) {
                                status.STATE = START;
                                setVisibleTiles = false;
                            }
                            status.BOMBS = defaultStatus.BOMBS;
                            status.VISIBLE_TILES = defaultStatus.VISIBLE_TILES;
                            initializeBoard(board, status.W_TILES, status.H_TILES);
                            generateBombs(board, status.BOMBS, status);
                            generateNumbers(board, &status);
                        }

                        if (e.key.keysym.sym == SDLK_o) {
                            setVisibleTiles = !setVisibleTiles;
                        }

                        if (e.key.keysym.sym == SDLK_p) {
                            printMessage = !printMessage;
                        }

                        if (e.key.keysym.sym == SDLK_k) {
                            if (status.STATE == START) {
                                while (board[rectX][rectY].TYPE != BLANK) {
                                    status.BOMBS = defaultStatus.BOMBS;
                                    status.VISIBLE_TILES = defaultStatus.VISIBLE_TILES;
                                    initializeBoard(board, status.W_TILES, status.H_TILES);
                                    generateBombs(board, status.BOMBS, status);
                                    generateNumbers(board, &status);
                                }
                                status.STATE = PLAYING;
                            }
                            if (board[rectX][rectY].MARK != CELL_FLAGGED) {
                                revealEmptyCells(board, rectX, rectY, &status);
                            }
                        }

                        if (e.key.keysym.sym == SDLK_l) {
                            if (status.STATE == PLAYING && (!board[rectX][rectY].VISIBLE || (setVisibleTiles && !board[rectX][rectY].VISIBLE))) {
                                board[rectX][rectY].MARK = (board[rectX][rectY].MARK + 1) % 3;
                            } else {
                                board[rectX][rectY].MARK = CELL_CLEARED;
                            }
                        }


                    }
                }

                if (cursorRect.x == status.WIDTH) {
                    cursorRect.x = 0;
                }

                if (cursorRect.x == -status.TILE) {
                    cursorRect.x = status.WIDTH - status.TILE;
                }

                if (cursorRect.y == status.HEIGHT) {
                    cursorRect.y = 0;
                }

                if (cursorRect.y == -status.TILE) {
                    cursorRect.y = status.HEIGHT - status.TILE;
                }


                // Initialize renderer color white for the background
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // Clear screen
                SDL_RenderClear(renderer);

                for (int x = 0; x < status.W_TILES; x++) {
                    for (int y = 0; y < status.H_TILES; y++) {
                        SDL_Rect rect = {x * status.TILE,
                                         y * status.TILE,
                                         status.TILE,
                                         status.TILE};
                        if (board[x][y].VISIBLE || setVisibleTiles) {
                            if (board[x][y].TYPE == BLANK) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[8], &rect);
                                // FIXME add && gameOver
                                if (board[x][y].MARK == CELL_FLAGGED) {
                                    SDL_RenderCopy(renderer, texture, &spriteRectangles[11], &rect);
                                }
                            }
                            if (board[x][y].TYPE == NUMBER) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[board[x][y].AMOUNT-1], &rect);
                                if (board[x][y].MARK == CELL_FLAGGED) {
                                    SDL_RenderCopy(renderer, texture, &spriteRectangles[11], &rect);
                                }
                            }
                            if (board[x][y].TYPE == MINE) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[14], &rect);
                            }
                            if (board[x][y].TYPE == MINE_EXPLOSION) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[15], &rect);
                            }
                        } else {
                            SDL_RenderCopy(renderer, texture, &spriteRectangles[9], &rect);
                            if (board[x][y].MARK == CELL_FLAGGED) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[10], &rect);
                            }
                            if (board[x][y].MARK == CELL_QUESTIONED) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[13], &rect);
                            }
                        }
                    }
                }

                SDL_RenderCopy(renderer, pointerTexture, NULL, &cursorRect);

                if (printMessage) {
                    message = renderText(renderer, font, (rectX), (rectY), textColor);
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

                    message = renderText(renderer, font, board[(rectX)][(rectY)].TYPE, board[(rectX)][(rectY)].AMOUNT, textColor);
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

                    message = renderText(renderer, font, board[(rectX)][(rectY)].MARK, board[(rectX)][(rectY)].VISIBLE, textColor);
                    if (message == NULL) {
                        TTF_CloseFont(font);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        TTF_Quit();
                        SDL_Quit();
                        return 1;
                    }
                    messageRect.y = 120;
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, status.STATE, 0, textColor);
                    if (message == NULL) {
                        TTF_CloseFont(font);
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        TTF_Quit();
                        SDL_Quit();
                        return 1;
                    }
                    messageRect.y = 190;
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


