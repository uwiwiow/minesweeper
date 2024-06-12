#include <stdbool.h>
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define handle_error(msg) \
           do { perror(msg); } while (0);

#define MAX_CLIENTS 100

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
    BLANK_TILE,
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
    float WIDTH;
    float HEIGHT;
    int W_TILES;
    int H_TILES;
    int TILE;
    int BOMBS;
    int VISIBLE_TILES;
    State STATE;
    CellType FIRST_CELL;
    unsigned int MAX_ITERATIONS;
} Status;

typedef enum CellAction {
    NONE = -1,
    OPENED,
    CLEAR,
    FLAGGED,
    QUESTIONED
} CellAction;

typedef struct Packet {
    Vector2 pos_cursor;
    CellAction action_tile;
} Packet;

void initializeBoard(TILE **board, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            board[i][j].TYPE = BLANK_TILE;
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

int main( int argc, char *argv[] )
{
    SetTraceLogLevel(LOG_WARNING);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        handle_error("socket");

    struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(12345),
            .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        handle_error("connect");

    time_t t;
    srand(123);

    Status status = {
            .TILE = 40,
            .W_TILES = 32,
            .H_TILES = 16,
            .BOMBS = 99,
            .STATE = START,
            .VISIBLE_TILES = 0,
            .FIRST_CELL = BLANK_TILE,
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

    InitWindow(status.WIDTH, status.HEIGHT, "Minesweeper");


    SetTargetFPS(60);

    // GENERATE BOMBS And NUMBERS
    generateBombs(board, status.BOMBS, status);
    generateNumbers(board, &status);

    Image atlas = LoadImage("assets/atlas.png");
    Image cursorImg = LoadImage("assets/pointer.png");
    ImageResize(&cursorImg, status.TILE, status.TILE);
    Texture cursor = LoadTextureFromImage(cursorImg);
    UnloadImage(cursorImg);
    Vector2 cursorRect = {0, 0};

    // SPRITE ARRAY
    Texture sprites[16];

    int spriteIndex = 0;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            Rectangle rect = { col * 16, row * 16, 16, 16 };
            Image img = ImageFromImage(atlas, rect);
            ImageResize(&img, status.TILE, status.TILE);
            sprites[spriteIndex] = LoadTextureFromImage(img);
            UnloadImage(img);
            spriteIndex++;
        }
    }
    UnloadImage(atlas);


    // GENERAL VAR SETTINGS
    bool setVisibleTiles = false;

    // CURSOR RECT ON TILES
    int rectX, rectY;
    Vector2 cursors[MAX_CLIENTS];

    Packet packet = {
            cursorRect,
            NONE
    };
    Packet allPackets[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        allPackets[i] = (Packet) {
                cursorRect,
                NONE
        };
    }

    while (!WindowShouldClose()) {


        // CURSOR MOVEMENT
        if (IsKeyPressed(KEY_A)) {
            cursorRect.x -= status.TILE;
        }

        if (IsKeyPressed(KEY_D)) {
            cursorRect.x += status.TILE;
        }

        if (IsKeyPressed(KEY_W)) {
            cursorRect.y -= status.TILE;
        }

        if (IsKeyPressed(KEY_S)) {
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

        packet.pos_cursor = cursorRect;
        packet.action_tile = NONE;


        // GAMEPLAY
        if (IsKeyPressed(KEY_O)) {
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

        if (IsKeyPressed(KEY_P)) {
            setVisibleTiles = !setVisibleTiles;
        }

        if (IsKeyPressed(KEY_K)) {
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
                        CloseWindow();
                        break;
                    }

                }
                status.STATE = PLAYING;
            }
            if (board[rectX][rectY].MARK != CELL_FLAGGED && (status.STATE == START || status.STATE == PLAYING)) {
                revealEmptyCells(board, rectX, rectY, &status);
                packet.action_tile = OPENED;
            }
        }

        if (IsKeyPressed(KEY_L)) {
            if (status.STATE == PLAYING && (!board[rectX][rectY].VISIBLE || (setVisibleTiles && !board[rectX][rectY].VISIBLE))) {
                board[rectX][rectY].MARK = (board[rectX][rectY].MARK + 1) % 3;
                packet.action_tile = (board[rectX][rectY].MARK) + 1;
            } else {
                board[rectX][rectY].MARK = CELL_CLEARED;
            }
        }

        if (send(socket_fd, &packet, sizeof(packet), 0) == -1)
            handle_error("send")

//        printf("Sent Packet: Cursor(%f, %f),Action(%d)\n",
//               packet.pos_cursor.x, packet.pos_cursor.y, packet.action_tile);

        ssize_t nbytes_read = recv(socket_fd, allPackets, sizeof(allPackets), 0);
        if (nbytes_read == -1)
            handle_error("recv");

        int num_packets_received = nbytes_read / sizeof(Packet);
//        printf("Received %d Packets from server:\n", num_packets_received);
        for (int i = 0; i < num_packets_received; i++) {
            if (packet.action_tile >= 0 ) {
                printf("Packet %d: Cursor(%f, %f),Action(%d)\n",
                       i, allPackets[i].pos_cursor.x, allPackets[i].pos_cursor.y, allPackets[i].action_tile);
            }

            int pkRectX = allPackets[i].pos_cursor.x/status.TILE;
            int pkRectY = allPackets[i].pos_cursor.y/status.TILE;

            switch (allPackets[i].action_tile) {
                case NONE:
                    break;
                case OPENED: {
                    iterationCounter = 0;
                    if (status.STATE == START && status.FIRST_CELL != ANY) {
                        while (board[pkRectX][pkRectY].TYPE != status.FIRST_CELL) {
                            status.BOMBS = defaultStatus.BOMBS;
                            status.VISIBLE_TILES = defaultStatus.VISIBLE_TILES;
                            initializeBoard(board, status.W_TILES, status.H_TILES);
                            generateBombs(board, status.BOMBS, status);
                            generateNumbers(board, &status);

                            iterationCounter++;
                            if (iterationCounter > status.MAX_ITERATIONS) {
                                CloseWindow();
                                break;
                            }

                        }
                        status.STATE = PLAYING;
                    }
                    if (board[rectX][rectY].MARK != CELL_FLAGGED && (status.STATE == START || status.STATE == PLAYING)) {
                        revealEmptyCells(board, pkRectX, pkRectY, &status);
                    }
                }
                    break;
                case CLEAR: board[pkRectX][pkRectY].MARK = CELL_CLEARED;
                    break;
                case FLAGGED: board[pkRectX][pkRectY].MARK = CELL_FLAGGED;
                    break;
                case QUESTIONED: board[pkRectX][pkRectY].MARK = CELL_QUESTIONED;
                    break;
            }

        }


        BeginDrawing();

        ClearBackground(RAYWHITE);

        // RENDER TILES
        for (int x = 0; x < status.W_TILES; x++) {
            for (int y = 0; y < status.H_TILES; y++) {
                // SET RECT FOR ALL TILES
                Vector2 rect = {x * status.TILE,
                                y * status.TILE};

                if (board[x][y].VISIBLE || setVisibleTiles || status.STATE == LOSE || status.STATE == WIN) {

                    if (board[x][y].TYPE == BLANK_TILE) {
                        DrawTextureV(sprites[8], rect, WHITE);
                        if (board[x][y].MARK == CELL_FLAGGED && status.STATE == LOSE) {
                            DrawTextureV(sprites[11], rect, WHITE);
                        }
                    }

                    if (board[x][y].TYPE == NUMBER) {
                        DrawTextureV(sprites[board[x][y].AMOUNT-1], rect, WHITE);

                        if (board[x][y].MARK == CELL_FLAGGED && status.STATE == LOSE) {
                            DrawTextureV(sprites[11], rect, WHITE);
                        }
                    }

                    if (board[x][y].TYPE == MINE) {
                        DrawTextureV(sprites[14], rect, WHITE);
                    }

                    if (board[x][y].TYPE == MINE_EXPLOSION) {
                        DrawTextureV(sprites[15], rect, WHITE);
                    }
                } else {

                    DrawTextureV(sprites[9], rect, WHITE);

                    if (board[x][y].MARK == CELL_FLAGGED) {
                        DrawTextureV(sprites[10], rect, WHITE);
                    }

                    if (board[x][y].MARK == CELL_QUESTIONED) {
                        DrawTextureV(sprites[13], rect, WHITE);
                    }
                }
            }
        }

        // RENDER CURSOR
        for (int p = 0; p < num_packets_received; p++) {
            DrawTextureV(cursor, allPackets[p].pos_cursor, WHITE);
        }

        EndDrawing();

    }

    for (int i = 0; i < 16; i++) {
        UnloadTexture(sprites[i]);
    }
    UnloadTexture(cursor);
    CloseWindow();          // Close window and OpenGL context

    freeMem(status, board);

    if (close(socket_fd) == -1)
        handle_error("close");



    return 0;

}


