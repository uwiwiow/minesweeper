#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* intToString(int number) {
    char* result = (char*)malloc(14 * sizeof(char));
    sprintf(result, "  %d", number);
    return result;
}

char* XYToString(int x, int y) {
    char* result = malloc(20 * sizeof(char));
    sprintf(result, "(%d, %d)", x, y);
    return result;
}

char* boolToString(bool b) {
    return b ? "  true" : "  false";
}

char* markToString(CellMark mark) {
    switch (mark) {
        case CELL_CLEARED:
            return "CLEAR";
        case CELL_FLAGGED:
            return "FLAG";
        case CELL_QUESTIONED:
            return "QSTN";
        default:
            return "UNKN";
    }
}

char* typeToString(CellType type) {
    switch (type) {
        case BLANK:
            return "BLANK";
        case NUMBER:
            return "NUMBER";
        case MINE:
            return "MINE";
        case MINE_EXPLOSION:
            return "M-EXP";
        default:
            return "UNKN";
    }
}

char* stateToString(State state) {
    switch (state) {
        case START:
            return "START";
        case PLAYING:
            return "PLAYING";
        case WIN:
            return "WIN";
        case LOSE:
            return "LOSE";
        default:
            return "UNKN";
    }
}

char* combineStrings(char* str1, bool freeText1, char* str2, bool freeText2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t totalLen = len1 + len2 + 2;

    char* result = (char*)malloc(totalLen * sizeof(char));
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    sprintf(result, "%s %s", str1, str2);

    if (freeText1) {
        free(str1);
    }
    if (freeText2) {
        free(str2);
    }

    return result;
}

SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, char* text, SDL_Color fgColor, SDL_Color bgColor, bool freeText) {
    SDL_Surface* surfaceMessage;

    if (bgColor.a == 0) {
        surfaceMessage = TTF_RenderText_Blended(font, text, fgColor);
    } else {
        surfaceMessage = TTF_RenderText_Shaded(font, text, fgColor, bgColor);
    }

    if (surfaceMessage == NULL) {
        printf("TTF_RenderText Error: %s\n", TTF_GetError());
        if (freeText) {
            free(text);
        }
        return NULL;
    }

    SDL_Texture* textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_FreeSurface(surfaceMessage);
    if (textureMessage == NULL) {
        printf("CreateTexture Error: %s\n", SDL_GetError());
        if (freeText) {
            free(text);
        }
        return NULL;
    }

    if (freeText) {
        free(text);
    }

    return textureMessage;
}

bool renderTextFail(SDL_Texture* message, SDL_Texture *cursorTexture, TTF_Font *font, SDL_Renderer *renderer, SDL_Window *window, Status status, TILE** board) {
    if (message == NULL) {
        if (cursorTexture != NULL) {
            SDL_DestroyTexture(cursorTexture);
        }
        freeMem(status, board);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return true;
    }
    return false;
}
