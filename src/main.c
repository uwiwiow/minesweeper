#include <stdbool.h>
#include <raylib.h>
#include <stdlib.h>
#include "board.h"
#include "texture.h"
#include "log.h"
#include <time.h>


struct BoardConfig board_config;
Texture cursor;
Texture sprites[16];

void cleanup() {
    freeTextures(cursor, sprites);
}

int main() {

    srand(time(NULL));

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(640, 480, "minesweeper");

    /*
     * Declare board config
     * */
    board_config = (struct BoardConfig) {
        .x = 50,
        .y = 50,
        .rows = 5,
        .columns = 8,
        .c_size = 40,
        .state = WIN};
    board_config.total_c = board_config.columns * board_config.rows;

    /*
     * Declare Board
     * */
    struct Cell board[board_config.rows][board_config.columns];
    initBoard(board_config, board);
    error(board == NULL, "board")

    /*
     * Get Textures
     * */
    getTextures(board_config.c_size, &cursor, sprites);
    atexit(cleanup);

    while(!WindowShouldClose()) {

        if (IsKeyDown(KEY_ONE)) {
            board_config.state = WIN;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        drawBoard(&board_config, board, sprites);

        EndDrawing();

    }

    CloseWindow();

    for (int i = 0; i < board_config.rows; i++) {
        for (int j = 0; j < board_config.columns; j++) {
            printf("%d ", board[i][j].c_t);
        }
        printf("\n");
    }


    return 0;
}