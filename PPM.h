#pragma once
#include "Vec3.h"

#define THREADING 0

#if THREADING
#include <pthread.h>
#endif

typedef struct
{
    int width;
    int height;
    unsigned char* data;
    #if THREADING
    pthread_mutex_t mutex;
    #endif
} Image;

typedef Vec3 Color;

Image createImage(int w, int h);
void deleteImage(Image* image);
void setPixel(Image* image, int x, int y, Color* color);
#if THREADING
void setPixelThreaded(Image* image, int x, int y, Color* color);
#endif
void outputImage(Image* image, const char* file);

Color randomColor();
