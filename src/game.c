#include "game.h"

TILE **mallocBoard(int rows, int cols) {
    TILE **board = malloc(rows * sizeof(TILE *));
    if (board == NULL) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        board[i] = malloc(cols * sizeof(TILE));
        if (board[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(board[j]);
            }
            free(board);
            return NULL;
        }
    }

    return board;
}
