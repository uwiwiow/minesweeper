#include "game.h"
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

    if (!initializeSDL(&status, board)) {
        return -1;
    }

    gameLoop(&status, defaultStatus, iterationCounter, board);

    cleanupSDL(status, board);

    return 0;
}
