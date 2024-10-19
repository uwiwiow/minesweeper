#include <stdbool.h>
#include <raylib.h>
#include <stdlib.h>
#include "texture.h"

#define info(FORMAT, ...)                                                                                              \
    fprintf(stdout, "\n%s -> %s():%i \n\t" FORMAT "\n", __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);         \

#define error(ERROR, FORMAT, ...)                                                                                      \
    if (ERROR) {                                                                                                       \
    fprintf(stderr, "\033[1;31m\n%s -> %s():%i -> Error(%i):\n\t%s\n\t" FORMAT "\n",                                   \
    __FILE_NAME__, __FUNCTION__, __LINE__, errno, strerror(errno), ##__VA_ARGS__);                                     \
    exit(EXIT_FAILURE);                                                                                                \
}

struct Cell {
    int c_t; // cell type: 0 none, 1 - 8 number, -1 bomb
    int  c_s; // cell status: 0 closed, 1 open, 2 flag, 3 qstn
};

struct BoardConfig {
    int x, y;
    int rows;
    int columns;
    int total_c;
    int c_size;
};

struct BoardConfig board_config;

void main() {

    board_config = (struct BoardConfig) {.x = 50, .y = 50, .rows = 5, .columns = 5, .c_size = 40};
    board_config.total_c = board_config.columns * board_config.rows;

    Texture cursor;
    Texture sprites[16];
    getTextures(board_config.c_size, &cursor, sprites);

    struct Cell cell_array[board_config.total_c];

    for (int i = 0; i < board_config.total_c; i++) {
        cell_array[i] = (struct Cell) {.c_t = 0, .c_s = 0};
    }
    
    InitWindow(480, 640, "minesweeper");

    while(!WindowShouldClose()) {
        
    }

    CloseWindow();
    
    
}