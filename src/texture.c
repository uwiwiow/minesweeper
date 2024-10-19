#include "texture.h"

int getTextures(int size, Texture *cursor, Texture *sprites) {
    const Image atlas = LoadImage("assets/atlas.png");
    Image cursorImg = LoadImage("assets/pointer.png");
    ImageResize(&cursorImg, size, size);
    *cursor = LoadTextureFromImage(cursorImg);
    UnloadImage(cursorImg);

    int spriteIndex = 0;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            const Rectangle rect = { (float) col * 16, (float) row * 16, 16, 16 };
            Image img = ImageFromImage(atlas, rect);
            ImageResize(&img, size, size);
            sprites[spriteIndex] = LoadTextureFromImage(img);
            UnloadImage(img);
            spriteIndex++;
        }
    }
    UnloadImage(atlas);

    return 0;
}

void freeTextures(Texture *cursor, Texture *sprites) {

}