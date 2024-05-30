#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_main.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

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

TILE **mallocBoard(int rows, int cols);
void initializeBoard(TILE **board, int width, int height);
void freeMem(Status status, TILE **board);
void generateBombs(TILE **board, int count, Status status);
void generateNumbers(TILE **board, Status *status);
void revealEmptyCells(TILE **board, int x, int y, Status *status);

char* intToString(int number);
char* XYToString(int x, int y);
char* boolToString(bool b);
char* markToString(CellMark mark);
char* typeToString(CellType type);
char* stateToString(State state);
char* combineStrings(char* str1, bool freeText1, char* str2, bool freeText2);

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color fgColor, SDL_Color bgColor, bool freeText);
bool renderTextFail(SDL_Texture* message, SDL_Texture *cursorTexture, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window, Status status, TILE** board);

#endif // GAME_H
