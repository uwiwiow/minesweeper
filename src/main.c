#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {

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

    TILE **board = mallocBoard(status.W_TILES, status.H_TILES);
    if (board == NULL) {
        return -1;
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
                            if (status.STATE != defaultStatus.STATE) {
                                status.STATE = defaultStatus.STATE;
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
