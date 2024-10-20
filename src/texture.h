#ifndef TEXTURE_H
#define TEXTURE_H

#include <raylib.h>
#include <stdio.h>

int getTextures(int size, Texture *cursor, Texture *sprites);

void freeTextures(Texture cursor, Texture *sprites);

#endif //TEXTURE_H
