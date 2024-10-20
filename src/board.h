#ifndef MINESWEEPER_BOARD_H
#define MINESWEEPER_BOARD_H

#include <raylib.h>

enum cellSprites {ONE_S, TWO_S, THREE_S, FOUR_S, FIVE_S, SIX_S, SEVEN_S, EIGHT_S,
        CLNO_S /* CLEAN OPEN */,
        CLNC_S /* CLEAN CLOSED (ANY CLOSED) */,
        BLNK_S /* CLEAN CLOSED (ANY CLOSED) */ = 9,
        FLGC_S /* FLAG CLOSED */,
        FLGO_S /* FLAG OPEN (NOT A BOMB) */,
        QSTNO_S __attribute__((unused))  /* QUESTION OPEN (?) */,
        QSTNC_S /* QUESTION CLOSED */,
        BMBOW_S /* BOMB OPEN WHITE (GAMEOVER || WIN) */,
        BMBOR_S /* BOMB OPEN RED (GAMEOVER) */
};


enum cellType {BOMB = -1, NONE, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT};
enum cellStatus {CLOSED, OPEN, FLAG, QSTN};
struct Cell {
    enum cellType c_t; // cell type: 0 none, 1 - 8 number, -1 bomb
    enum cellStatus c_s; // cell status: 0 closed, 1 open, 2 flag, 3 qstn
};


enum boardState {PAUSE, START, PLAYING, GAMEOVER, WIN};
struct BoardConfig {
    int x, y; // position on the screen to start drawing the board
    int rows; // number of cells in a row
    int columns; // number of cells in a column
    int total_c; // total cells
    int c_size; // cell size in pixels
    enum boardState state; // game state: 0 pause, 1 start, 2 playing, 3 gameover, 4 win
};


int initBoard(struct BoardConfig board_config, struct Cell board[board_config.rows][board_config.columns]);
int drawBoard(struct BoardConfig *board_config, struct Cell board[board_config->rows][board_config->columns], Texture *sprites);


#endif //MINESWEEPER_BOARD_H
