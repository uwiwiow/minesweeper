#include "game.h"
#include <stdlib.h>

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
