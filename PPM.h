#pragma once
#include "Vec3.h"

typedef struct
{
    int width;
    int height;
    unsigned char* data;
} Image;

typedef Vec3 Color;

Image createImage(int w, int h);
void deleteImage(Image image);
void setPixel(Image image, int x, int y, Color color);
void outputImage(Image image, const char* file);
Color randomColor();