#include "board.h"
#include "log.h"

Vector2 getCellPos(struct BoardConfig board_config, int row, int col) {
    return (Vector2) {(float) board_config.x + ( (float) board_config.c_size * (float) row),
                      (float) board_config.y + ( (float) board_config.c_size * (float) col)};
}

int initBoard(struct BoardConfig board_config, struct Cell board[board_config.rows][board_config.columns]) {
    for (int i = 0; i < board_config.rows; i++) {
        for (int j = 0; j < board_config.columns; j++) {
            board[i][j] = (struct Cell) {.c_t = (rand() % 10) - 1, .c_s = CLOSED};
            printf("%d ", board[i][j].c_t);
        }
        printf("\n");
    }
    return 0;
}

int drawBoard(struct BoardConfig *board_config, struct Cell board[board_config->rows][board_config->columns], Texture *sprites) {
// TODO draw only affected cells
// TODO !# To win a game of Minesweeper, all non-mine cells must be opened without opening a mine. (wikipedia)
    for (int i = 0; i < board_config->rows; i++) {
        for (int j = 0; j < board_config->columns; j++) {

            switch (board_config->state) {
                case PAUSE:
                    // todo idk wtf should go here

                case START:
                    DrawTextureV(sprites[CLNC_S], getCellPos(*board_config, i, j), WHITE);
                    break;

                case PLAYING:
                    switch (board[i][j].c_t) {
                        case BOMB:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    board_config->state = GAMEOVER;
                                    DrawTextureV(sprites[BMBOR_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case NONE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[CLNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[CLNO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case ONE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[ONE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case TWO:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[TWO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case THREE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[THREE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case FOUR:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[FOUR_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case FIVE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[FIVE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case SIX:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[SIX_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case SEVEN:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[SEVEN_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case EIGHT:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                    DrawTextureV(sprites[BLNK_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[EIGHT_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case QSTN:
                                    DrawTextureV(sprites[QSTNC_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                    }
                    break;

                case WIN:
                case GAMEOVER:
                    switch (board[i][j].c_t) {
                        case BOMB:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case FLAG:
                                case QSTN:
                                    DrawTextureV(sprites[BMBOW_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case OPEN:
                                    DrawTextureV(sprites[BMBOR_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case NONE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[CLNO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case ONE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[ONE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case TWO:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[TWO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case THREE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[THREE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case FOUR:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[FOUR_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case FIVE:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[FIVE_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case SIX:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[SIX_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case SEVEN:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[SEVEN_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                        case EIGHT:
                            switch (board[i][j].c_s) {
                                case CLOSED:
                                case OPEN:
                                case QSTN:
                                    DrawTextureV(sprites[EIGHT_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                                case FLAG:
                                    DrawTextureV(sprites[FLGO_S], getCellPos(*board_config, i, j), WHITE);
                                    break;
                            }
                            break;
                    }
                    break;
            }


        }
    }
    return 0;

}