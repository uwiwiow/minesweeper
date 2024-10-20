#include "texture.h"

int getTextures(int size, Texture *cursor, Texture *sprites) {
    Image cursorImg = LoadImage("assets/pointer.png");
    ImageResize(&cursorImg, size, size);
    *cursor = LoadTextureFromImage(cursorImg);
    UnloadImage(cursorImg);

    const Image atlas = LoadImage("assets/atlas.png");
    int spriteIndex = 0;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col, spriteIndex++) {
            const Rectangle rect = { (float) col * 16, (float) row * 16, 16, 16 };
            Image img = ImageFromImage(atlas, rect);
            ImageResize(&img, size, size);
            sprites[spriteIndex] = LoadTextureFromImage(img);
            UnloadImage(img);
        }
    }
    UnloadImage(atlas);

    return 0;
}

void freeTextures(Texture cursor, Texture *sprites) {

    UnloadTexture(cursor);

    for (int i = 0; i < 16; i++) {
        UnloadTexture(sprites[i]);
    }

}