#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <time.h>

typedef enum State {
    START,
    PLAYING,
    WIN,
    LOSE
} State;

typedef enum CellMark {
    CELL_CLEARED,
    CELL_FLAGGED,
    CELL_QUESTIONED
} CellMark;

typedef enum CellType {
    BLANK,
    NUMBER,
    MINE,
    MINE_EXPLOSION,
    ANY
} CellType;

typedef struct TILE {
    CellType TYPE;
    int AMOUNT;
    CellMark MARK;
    bool VISIBLE;
} TILE;

typedef struct Status {
    int WIDTH;
    int HEIGHT;
    int W_TILES;
    int H_TILES;
    int TILE;
    int BOMBS;
    int VISIBLE_TILES;
    State STATE;
    CellType FIRST_CELL;
    unsigned int MAX_ITERATIONS;
} Status;

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

char* intToString(int number) {
    char* result = (char*)malloc(14 * sizeof(char));
    sprintf(result, "  %d", number);
    return result;
}

char* XYToString(int x, int y) {
    char* result = malloc(20 * sizeof(char));
    sprintf(result, "(%d, %d)", x, y);
    return result;
}

char* boolToString(bool b) {
    return b ? "  true" : "  false";
}

char* markToString(CellMark mark) {
    switch (mark) {
        case CELL_CLEARED:
            return "CLEAR";
        case CELL_FLAGGED:
            return "FLAG";
        case CELL_QUESTIONED:
            return "QSTN";
        default:
            return "UNKN";
    }
}

char* typeToString(CellType type) {
    switch (type) {
        case BLANK:
            return "BLANK";
        case NUMBER:
            return "NUMBER";
        case MINE:
            return "MINE";
        case MINE_EXPLOSION:
            return "M-EXP";
        default:
            return "UNKN";
    }
}

char* stateToString(State state) {
    switch (state) {
        case START:
            return "START";
        case PLAYING:
            return "PLAYING";
        case WIN:
            return "WIN";
        case LOSE:
            return "LOSE";
        default:
            return "UNKN";
    }
}

char* combineStrings(char* str1, bool freeText1, char* str2, bool freeText2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t totalLen = len1 + len2 + 2;

    char* result = (char*)malloc(totalLen * sizeof(char));
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    sprintf(result, "%s %s", str1, str2);

    if (freeText1) {
        free(str1);
    }
    if (freeText2) {
        free(str2);
    }

    return result;
}

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color fgColor, SDL_Color bgColor, bool freeText) {
    SDL_Surface* surfaceMessage;

    if (bgColor.a == 0) {
        surfaceMessage = TTF_RenderText_Blended(font, text, fgColor);
    } else {
        surfaceMessage = TTF_RenderText_Shaded(font, text, fgColor, bgColor);
    }

    if (surfaceMessage == NULL) {
        printf("TTF_RenderText Error: %s\n", TTF_GetError());
        if (freeText) {
            free(text);
        }
        return NULL;
    }

    SDL_Texture* textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if (textureMessage == NULL) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        if (freeText) {
            free(text);
        }
        return NULL;
    }

    if (freeText) {
        free(text);
    }

    return textureMessage;
}

bool renderTextFail(SDL_Texture* message, SDL_Texture *cursorTexture, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window, Status status, TILE** board) {
    if (message == NULL) {
        SDL_DestroyTexture(cursorTexture);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        freeMem(status, board);
        return true;
    }
    return false;
}

int main( int argc, char *argv[] )
{

    time_t t;
    srand((unsigned)time(&t));

    Status status = {
        .TILE = 40,
        .W_TILES = 32,
        .H_TILES = 16,
        .BOMBS = 99,
        .STATE = START,
        .VISIBLE_TILES = 0,
        .FIRST_CELL = BLANK,
        .MAX_ITERATIONS = 10000
    };
    status.WIDTH = status.W_TILES * status.TILE;
    status.HEIGHT = status.H_TILES * status.TILE;

    unsigned int iterationCounter = 0;

    Status defaultStatus = status;

    TILE **board = malloc(status.W_TILES * sizeof(TILE *));
    for (int i = 0; i < status.W_TILES; i++) {
        board[i] = malloc(status.H_TILES * sizeof(TILE));
    }

    initializeBoard(board, status.W_TILES, status.H_TILES);

    SDL_SetMainReady();

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
            SDL_Texture* cursorTexture = SDL_CreateTextureFromSurface(renderer, pointerSurface);
            SDL_FreeSurface(pointerSurface);
            if (cursorTexture == NULL) {
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

            // SPRITE ARRAY
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
            SDL_Color bgColor = {255, 255, 255, 255};
            SDL_Color textColor = {0, 0, 0, 255};
            SDL_Rect messageRect = {0, -13, 100, 60};
            SDL_Texture* message = renderText(renderer, font, "xxx", textColor, bgColor, false);
            renderTextFail(message, cursorTexture, font, renderer, window, status, board);
            bool printMessage1 = false;
            bool printMessage2 = false;
            bool printMessage3 = false;
            bool printMessage4 = false;

            int message1 = 0;
            int message2 = 0;
            int message3 = 0;


            // GENERAL VAR SETTINGS
            bool quit = false;
            bool setVisibleTiles = false;

            // CURSOR RECT ON TILES
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


                        // CURSOR MOVEMENT
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

                        // CURSOR DOESN'T GO OUTSIDE THE WINDOW
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

                        rectX = cursorRect.x/status.TILE;
                        rectY = cursorRect.y/status.TILE;


                        // DEBUG
                        if (e.key.keysym.sym == SDLK_1) {
                            printMessage1 = !printMessage1;
                        }

                        if (e.key.keysym.sym == SDLK_2) {
                            printMessage2 = !printMessage2;
                        }

                        if (e.key.keysym.sym == SDLK_3) {
                            printMessage3 = !printMessage3;
                        }

                        if (e.key.keysym.sym == SDLK_4) {
                            printMessage4 = !printMessage4;
                        }

                        if (e.key.keysym.sym == SDLK_5) {
                            bgColor.a = bgColor.a == 0 ? 255 : 0;
                        }


                        // GAMEPLAY
                        if (e.key.keysym.sym == SDLK_o) {
                            if (status.STATE != START) {
                                status.STATE = START;
                                setVisibleTiles = false;
                            }
                            status.BOMBS = defaultStatus.BOMBS;
                            status.VISIBLE_TILES = defaultStatus.VISIBLE_TILES;
                            initializeBoard(board, status.W_TILES, status.H_TILES);
                            generateBombs(board, status.BOMBS, status);
                            generateNumbers(board, &status);
                        }

                        if (e.key.keysym.sym == SDLK_p) {
                            setVisibleTiles = !setVisibleTiles;
                        }

                        if (e.key.keysym.sym == SDLK_k) {
                            iterationCounter = 0;
                            if (status.STATE == START && status.FIRST_CELL != ANY) {
                                while (board[rectX][rectY].TYPE != status.FIRST_CELL) {
                                    status.BOMBS = defaultStatus.BOMBS;
                                    status.VISIBLE_TILES = defaultStatus.VISIBLE_TILES;
                                    initializeBoard(board, status.W_TILES, status.H_TILES);
                                    generateBombs(board, status.BOMBS, status);
                                    generateNumbers(board, &status);

                                    iterationCounter++;
                                    if (iterationCounter > status.MAX_ITERATIONS) {
                                        quit = true;
                                        break;
                                    }

                                }
                                status.STATE = PLAYING;
                            }
                            if (board[rectX][rectY].MARK != CELL_FLAGGED && (status.STATE == START || status.STATE == PLAYING)) {
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


                // Initialize renderer color white for the background
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // Clear screen
                SDL_RenderClear(renderer);

                // RENDER TILES
                for (int x = 0; x < status.W_TILES; x++) {
                    for (int y = 0; y < status.H_TILES; y++) {
                        // SET RECT FOR ALL TILES
                        SDL_Rect rect = {x * status.TILE,
                                         y * status.TILE,
                                         status.TILE,
                                         status.TILE};

                        if (board[x][y].VISIBLE || setVisibleTiles || status.STATE == LOSE || status.STATE == WIN) {

                            if (board[x][y].TYPE == BLANK) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[8], &rect);

                                if (board[x][y].MARK == CELL_FLAGGED && status.STATE == LOSE) {
                                    SDL_RenderCopy(renderer, texture, &spriteRectangles[11], &rect);
                                }
                            }

                            if (board[x][y].TYPE == NUMBER) {
                                SDL_RenderCopy(renderer, texture, &spriteRectangles[board[x][y].AMOUNT-1], &rect);

                                if (board[x][y].MARK == CELL_FLAGGED && status.STATE == LOSE) {
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

                // RENDER CURSOR
                SDL_RenderCopy(renderer, cursorTexture, NULL, &cursorRect);

                // RENDER DEBUG MESSAGES
                if (printMessage1) {
                    message1 = 0;
                    message = renderText(renderer, font, "  X  Y  ", textColor, bgColor, false);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){0, -10, 90, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, XYToString(rectX, rectY), textColor, bgColor, true);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){0, 23, 90, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);
                } else {
                    message1 = 120;
                }

                if (printMessage2) {
                    message2 = 0;
                    message = renderText(renderer, font, "TYPE - AMNT", textColor, bgColor, false);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){120 - message1, -10, 160, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, combineStrings(typeToString(board[(rectX)][(rectY)].TYPE), false,
                                                                        intToString(board[(rectX)][(rectY)].AMOUNT), true), textColor, bgColor, true);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){120 - message1, 23, 140, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);
                } else {
                    message2 = 190;
                }

                if (printMessage3) {
                    message3 = 0;
                    message = renderText(renderer, font, "MARK - VISIBLE", textColor, bgColor, false);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){310 - message1  - message2, -10, 160, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, combineStrings(markToString(board[(rectX)][(rectY)].MARK), false,
                                                                        boolToString(board[(rectX)][(rectY)].VISIBLE), false), textColor, bgColor, true);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){310 - message1  - message2, 23, 140, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);
                } else {
                    message3 = 190;
                }

                if (printMessage4) {
                    message = renderText(renderer, font, "STATE", textColor, bgColor, false);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){500 - message1  - message2  - message3, -10, 80, 40};
                    SDL_RenderCopy(renderer, message, NULL, &messageRect);

                    message = renderText(renderer, font, stateToString(status.STATE), textColor, bgColor, false);
                    if (renderTextFail(message, cursorTexture, font, renderer, window, status, board)){
                        return 1;
                    }
                    messageRect = (SDL_Rect){500 - message1  - message2  - message3, 23, 80, 40};
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
            SDL_DestroyTexture(cursorTexture);
            TTF_CloseFont(font);

            // Destroy renderer
            SDL_DestroyRenderer(renderer);

        }

        // Destroy window
        SDL_DestroyWindow(window);

    }

    // Quit SDL
    TTF_Quit();
    SDL_Quit();

    freeMem(status, board);


    return 0;

}


